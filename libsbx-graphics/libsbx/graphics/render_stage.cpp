#include <libsbx/graphics/render_stage.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

class subpass_description {

public:

  subpass_description(VkPipelineBindPoint bind_point, std::vector<VkAttachmentReference>&& color_attachments, std::vector<VkAttachmentReference>&& input_attachments, const std::optional<std::uint32_t>& depth_attachment)
  : _color_attachment{std::move(color_attachments)},
    _input_attachments{std::move(input_attachments)} {
    _description = VkSubpassDescription{};
    _description.pipelineBindPoint = bind_point;
    _description.colorAttachmentCount = static_cast<std::uint32_t>(_color_attachment.size());
    _description.pColorAttachments = _color_attachment.data();
    
    if (!_input_attachments.empty()) {
      _description.inputAttachmentCount = static_cast<std::uint32_t>(_input_attachments.size());
      _description.pInputAttachments = _input_attachments.data();
    }

    if (depth_attachment) {
      _depth_attachment.attachment = *depth_attachment;
      _depth_attachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      _description.pDepthStencilAttachment = &_depth_attachment;
    }
  }

  auto description() const noexcept -> const VkSubpassDescription& {
    return _description;
  }

private:

  VkSubpassDescription _description;
  std::vector<VkAttachmentReference> _color_attachment;
  std::vector<VkAttachmentReference> _input_attachments;
  VkAttachmentReference _depth_attachment;

}; // class subpass_description

render_stage::render_stage(std::vector<graphics::attachment>&& attachments, std::vector<subpass_binding>&& subpass_bindings, const graphics::viewport& viewport)
: _attachments{std::move(attachments)}, 
  _subpass_bindings{std::move(subpass_bindings)},
  _viewport{viewport},
  _render_pass{nullptr},
  _subpass_attachment_counts{static_cast<std::uint32_t>(_subpass_bindings.size()), 0},
  _is_outdated{true} {

  for (const auto& attachment : _attachments) {
    auto clear_value = VkClearValue{};

    auto clear_color = attachment.clear_color();

    switch (attachment.image_type()) {
      case attachment::type::image: {
        clear_value.color = VkClearColorValue{clear_color.r, clear_color.g, clear_color.b, clear_color.a};

        for (const auto& subpass : _subpass_bindings) {
          if (auto bindings = subpass.attachment_bindings(); std::find(bindings.begin(), bindings.end(), attachment.binding()) != bindings.end()) {
            _subpass_attachment_counts[subpass.binding()]++;
          }
        }

        break;
      }
      case attachment::type::depth: {
        clear_value.depthStencil = VkClearDepthStencilValue{1.0f, 0};
        _depth_attachment = attachment;
        break;
      }
      case attachment::type::swapchain: {
        clear_value.color = VkClearColorValue{clear_color.r, clear_color.g, clear_color.b, clear_color.a};
        _swapchain_attachment = attachment;
        break;
      }
    }

    _clear_values.push_back(clear_value);
  }
}

render_stage::~render_stage() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  for (const auto& framebuffer : _framebuffers) {
    vkDestroyFramebuffer(logical_device, framebuffer, nullptr);
  }

  vkDestroyRenderPass(logical_device, _render_pass, nullptr);
}

auto render_stage::attachments() const noexcept -> const std::vector<graphics::attachment>& {
  return _attachments;
}

auto render_stage::find_attachment(const std::string& name) const noexcept -> std::optional<graphics::attachment> {
  auto entry = std::find_if(_attachments.begin(), _attachments.end(), [&name](const graphics::attachment& attachment) {
    return attachment.name() == name;
  });

  if (entry == _attachments.end()) {
    return std::nullopt;
  }

  return *entry;
}

auto render_stage::find_attachment(std::uint32_t binding) const noexcept -> std::optional<graphics::attachment> {
  auto entry = std::find_if(_attachments.begin(), _attachments.end(), [&binding](const graphics::attachment& attachment) {
    return attachment.binding() == binding;
  });

  if (entry == _attachments.end()) {
    return std::nullopt;
  }

  return *entry;
}

auto render_stage::subpasses() const noexcept -> const std::vector<subpass_binding>& {
  return _subpass_bindings;
}

auto render_stage::attachment_count(std::uint32_t subpass) -> std::uint32_t {
  return _subpass_attachment_counts[subpass];
}

auto render_stage::is_outdated() const noexcept -> bool {
  return _is_outdated;
}

auto render_stage::clear_values() const noexcept -> const std::vector<VkClearValue>& {
  return _clear_values;
}

auto render_stage::has_depth_attachment() const noexcept -> bool {
  return _depth_attachment.has_value();
}

auto render_stage::has_swapchain_attachment() const noexcept -> bool {
  return _swapchain_attachment.has_value();
}

auto render_stage::viewport() const noexcept -> const class viewport& {
  return _viewport;
}

auto render_stage::render_area() const noexcept -> const class render_area& {
  return _render_area;
}

auto render_stage::render_pass() const noexcept -> const VkRenderPass& {
  return _render_pass;
}

auto render_stage::update() -> void {
  auto& devices_module = core::engine::get_module<devices::devices_module>();

  auto last_render_area = _render_area;

  _render_area.set_offset(_viewport.offset());

  // [TODO] KAJ 2023-06-14 : This looks a bit scuffed. Maybe try to understand what it actually does and rewrite it.

  if (auto size = _viewport.size(); size) {
    _render_area.set_extent(math::vector2u{_viewport.scale() * (*size)});
  } else {
    auto& window = devices_module.window();
    auto window_size = math::vector2u{window.width(), window.height()};

    _render_area.set_extent(math::vector2u{_viewport.scale() * window_size});
  }

  _render_area.set_aspect_ratio(static_cast<std::float_t>(_render_area.extent().x) / static_cast<std::float_t>(_render_area.extent().y));
  _render_area.set_extent(_render_area.extent() + _render_area.offset());

  _is_outdated = last_render_area != _render_area;
}

auto render_stage::rebuild(const swapchain& swapchain) -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  update();

  auto& surface = graphics_module.surface();

  if (_depth_attachment) {
    _depth_image = std::make_unique<graphics::depth_image>(_render_area.extent(), VK_SAMPLE_COUNT_1_BIT);
  }

  if (!_render_pass) {
    _create_render_pass(_depth_image ? _depth_image->format() : VK_FORMAT_UNDEFINED, surface.format().format);
  }

  for (const auto& attachment : _attachments) {
    if (attachment.image_type() == attachment::type::image) {
      _image_attachments.insert({attachment.binding(), std::make_unique<graphics::image2d>(_render_area.extent(), attachment.format(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLE_COUNT_1_BIT)});
    } else if (attachment.image_type() == attachment::type::swapchain) {
      _image_attachments.insert({attachment.binding(), nullptr});
    }
  }

  _rebuild_framebuffers(swapchain);

  _descriptors.clear();

  auto where = _descriptors.end();

  for (const auto& attachment : _attachments) {
    if (attachment.image_type() == attachment::type::depth) {
      where = _descriptors.insert(where, {attachment.name(), _depth_image.get()});
    } else {
      where = _descriptors.insert(where, {attachment.name(), _image_attachments[attachment.binding()].get()});
    }
  }

  _is_outdated = false;
}

auto render_stage::framebuffer(std::uint32_t index) noexcept -> const VkFramebuffer& {
  return _framebuffers[index];
}

auto render_stage::descriptor(const std::string& name) const noexcept -> const graphics::descriptor* {
  if (auto it = _descriptors.find(name); it != _descriptors.end()) {
    return it->second;
  }

  return nullptr;
}

auto render_stage::descriptors() const noexcept -> const std::map<std::string, const graphics::descriptor*>& {
  return _descriptors;
}

auto render_stage::_create_render_pass(VkFormat depth_format, VkFormat surface_format) -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  auto attachments = std::vector<VkAttachmentDescription>{};

  for (const auto& attachment : _attachments) {
    auto attachment_description = VkAttachmentDescription{};
    attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    switch (attachment.image_type()) {
      case attachment::type::image: {
        attachment_description.format = attachment.format();
        attachment_description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        break;
      }
      case attachment::type::depth: {
        attachment_description.format = depth_format;
        attachment_description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        break;
      }
      case attachment::type::swapchain: {
        attachment_description.format = surface_format;
        attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        break;
      }
    }

    attachments.push_back(attachment_description);
  }

  auto subpasses = std::vector<subpass_description>{};
  auto subpass_dependencies = std::vector<VkSubpassDependency>{};

  for (const auto& subpass : _subpass_bindings) {
		auto subpass_color_attachments = std::vector<VkAttachmentReference>{};
    auto subpass_input_attachments = std::vector<VkAttachmentReference>{};

		auto depth_attachment = std::optional<std::uint32_t>{};

		for (const auto& attachment_binding : subpass.attachment_bindings()) {
			auto image_attachment = find_attachment(attachment_binding);

			if (!image_attachment) {
				throw std::runtime_error{fmt::format("Failed to find color/depth attachment with binding {}", attachment_binding)};
			}

			if (image_attachment->image_type() == attachment::type::depth) {
				depth_attachment = image_attachment->binding();
				continue;
			}

			auto attachment_reference = VkAttachmentReference{};
			attachment_reference.attachment = image_attachment->binding();
			attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			subpass_color_attachments.push_back(attachment_reference);
		}

    for (const auto& attachment_binding : subpass.input_attachment_bindings()) {
			auto image_attachment = find_attachment(attachment_binding);

			if (!image_attachment) {
				throw std::runtime_error{fmt::format("Failed to find input attachment with binding {}", attachment_binding)};
			}

			auto attachment_reference = VkAttachmentReference{};
			attachment_reference.attachment = image_attachment->binding();
			attachment_reference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			subpass_input_attachments.push_back(attachment_reference);
		}

		subpasses.push_back(subpass_description{VK_PIPELINE_BIND_POINT_GRAPHICS, std::move(subpass_color_attachments), std::move(subpass_input_attachments), depth_attachment});

		auto subpass_dependency = VkSubpassDependency{};
		subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		subpass_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpass_dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		subpass_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    if (subpass.binding() == _subpass_bindings.size()) {
      subpass_dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
      subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      subpass_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      subpass_dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    } else {
      subpass_dependency.dstSubpass = subpass.binding();
    }

    if (subpass.binding() == 0) {
      subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
      subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
      subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpass_dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    } else {
      subpass_dependency.srcSubpass = subpass.binding() - 1;
    }

		subpass_dependencies.emplace_back(subpass_dependency);
	}

	auto subpass_descriptions = std::vector<VkSubpassDescription>{};
	subpass_descriptions.reserve(subpasses.size());

	for (const auto& subpass : subpasses) {
		subpass_descriptions.emplace_back(subpass.description());
	}

	auto render_pass_create_info = VkRenderPassCreateInfo{};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
	render_pass_create_info.pAttachments = attachments.data();
	render_pass_create_info.subpassCount = static_cast<std::uint32_t>(subpass_descriptions.size());
	render_pass_create_info.pSubpasses = subpass_descriptions.data();
	render_pass_create_info.dependencyCount = static_cast<std::uint32_t>(subpass_dependencies.size());
	render_pass_create_info.pDependencies = subpass_dependencies.data();

	validate(vkCreateRenderPass(logical_device, &render_pass_create_info, nullptr, &_render_pass));
}

auto render_stage::_rebuild_framebuffers(const swapchain& swapchain) -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  for (const auto& framebuffer : _framebuffers) {
    vkDestroyFramebuffer(logical_device, framebuffer, nullptr);
  }

  _framebuffers.resize(swapchain.image_count());

  for (auto i = 0u; i < _framebuffers.size(); ++i) {
    auto attachments = std::vector<VkImageView>{};

    for (const auto& attachment : _attachments) {
      switch (attachment.image_type()) {
        case attachment::type::image: {
          attachments.push_back(_image_attachments[attachment.binding()]->view());
          break;
        }
        case attachment::type::depth: {
          attachments.push_back(_depth_image->view());
          break;
        }
        case attachment::type::swapchain: {
          attachments.push_back(swapchain.image_view(i));
          break;
        }
        default: {
          break;
        }
      }
    }

    auto framebuffer_create_info = VkFramebufferCreateInfo{};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = _render_pass;
    framebuffer_create_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
    framebuffer_create_info.pAttachments = attachments.data();
    framebuffer_create_info.width = _render_area.extent().x;
    framebuffer_create_info.height = _render_area.extent().y;
    framebuffer_create_info.layers = 1;

    validate(vkCreateFramebuffer(logical_device, &framebuffer_create_info, nullptr, &_framebuffers[i]));
  }
}

} // namespace sbx::graphics
