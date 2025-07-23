// #include <libsbx/graphics/render_stage.hpp>

// #include <fmt/format.h>

// #include <range/v3/all.hpp>

// #include <libsbx/core/engine.hpp>

// #include <libsbx/graphics/graphics_module.hpp>

// namespace sbx::graphics {

// class subpass_description {

// public:

//   subpass_description(VkPipelineBindPoint bind_point, std::vector<VkAttachmentReference>&& color_attachments, std::vector<VkAttachmentReference>&& input_attachments, const std::optional<std::uint32_t>& depth_attachment)
//   : _color_attachments{std::move(color_attachments)},
//     _input_attachments{std::move(input_attachments)} {
//     _description = VkSubpassDescription{};
//     _description.pipelineBindPoint = bind_point;
//     _description.colorAttachmentCount = static_cast<std::uint32_t>(_color_attachments.size());
//     _description.pColorAttachments = _color_attachments.data();
    
//     if (!_input_attachments.empty()) {
//       _description.inputAttachmentCount = static_cast<std::uint32_t>(_input_attachments.size());
//       _description.pInputAttachments = _input_attachments.data();
//     }

//     if (depth_attachment) {
//       _depth_attachment.attachment = *depth_attachment;
//       _depth_attachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

//       _description.pDepthStencilAttachment = &_depth_attachment;
//     }
//   }

//   auto description() const noexcept -> const VkSubpassDescription& {
//     return _description;
//   }

// private:

//   VkSubpassDescription _description;
//   std::vector<VkAttachmentReference> _color_attachments;
//   std::vector<VkAttachmentReference> _input_attachments;
//   VkAttachmentReference _depth_attachment;

// }; // class subpass_description

// render_stage::render_stage(std::vector<graphics::attachment>&& attachments, std::vector<subpass_binding>&& subpass_bindings, const graphics::viewport& viewport)
// : _attachments{std::move(attachments)}, 
//   _subpass_bindings{std::move(subpass_bindings)},
//   _viewport{viewport},
//   _subpass_attachment_counts{},
//   _subpass_attachments{} {
//   auto attachment_counts = _subpass_bindings.size();

//   _subpass_attachment_counts.resize(attachment_counts, 0u);
//   _subpass_attachments.resize(attachment_counts);

//   for (const auto& attachment : _attachments) {
//     auto clear_value = VkClearValue{};

//     auto clear_color = attachment.clear_color();

//     switch (attachment.image_type()) {
//       case attachment::type::image:
//       case attachment::type::storage: {
//         clear_value.color = VkClearColorValue{clear_color.r(), clear_color.g(), clear_color.b(), clear_color.a()};

//         _update_subpass_attachment_counts(attachment);

//         break;
//       }
//       case attachment::type::depth: {
//         if (_depth_attachment) {
//           throw std::runtime_error{fmt::format("Render stage can at max have one depth attachment! Found depth attachments at bindings {} and {}", _depth_attachment->binding(), attachment.binding())};
//         }

//         clear_value.depthStencil = VkClearDepthStencilValue{1.0f, 0};
//         _depth_attachment = attachment;

//         break;
//       }
//       case attachment::type::swapchain: {
//         if (_swapchain_attachment) {
//           throw std::runtime_error{fmt::format("Render stage can at max have one swapchain attachment! Found swapchain attachments at bindings {} and {}", _swapchain_attachment->binding(), attachment.binding())};
//         }

//         _update_subpass_attachment_counts(attachment);

//         clear_value.color = VkClearColorValue{clear_color.r(), clear_color.g(), clear_color.b(), clear_color.a()};
//         _swapchain_attachment = attachment;

//         break;
//       }
//     }

//     _clear_values.push_back(clear_value);
//   }
// }

// render_stage::~render_stage() {
//   auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

//   auto& logical_device = graphics_module.logical_device();
// }

// auto render_stage::attachments() const noexcept -> const std::vector<graphics::attachment>& {
//   return _attachments;
// }

// auto render_stage::find_attachment(const std::string& name) const noexcept -> std::optional<graphics::attachment> {
//   auto entry = std::find_if (_attachments.begin(), _attachments.end(), [&name](const graphics::attachment& attachment) {
//     return attachment.name() == name;
//   });

//   if (entry == _attachments.end()) {
//     return std::nullopt;
//   }

//   return *entry;
// }

// auto render_stage::find_attachment(std::uint32_t binding) const noexcept -> std::optional<graphics::attachment> {
//   auto entry = std::find_if (_attachments.begin(), _attachments.end(), [&binding](const graphics::attachment& attachment) {
//     return attachment.binding() == binding;
//   });

//   if (entry == _attachments.end()) {
//     return std::nullopt;
//   }

//   return *entry;
// }

// auto render_stage::subpasses() const noexcept -> const std::vector<subpass_binding>& {
//   return _subpass_bindings;
// }

// auto render_stage::attachment_count(std::uint32_t subpass) const -> std::uint32_t {
//   return _subpass_attachment_counts[subpass];
// }

// auto render_stage::subpass_attachments(std::uint32_t subpass) const -> const std::vector<std::uint32_t>& {
//   return _subpass_attachments[subpass];
// }

// auto render_stage::clear_values() const noexcept -> const std::vector<VkClearValue>& {
//   return _clear_values;
// }

// auto render_stage::has_depth_attachment() const noexcept -> bool {
//   return _depth_attachment.has_value();
// }

// auto render_stage::has_swapchain_attachment() const noexcept -> bool {
//   return _swapchain_attachment.has_value();
// }

// auto render_stage::viewport() const noexcept -> const class viewport& {
//   return _viewport;
// }

// auto render_stage::render_area() const noexcept -> const class render_area& {
//   return _render_area;
// }

// auto render_stage::descriptor(const std::string& name) const noexcept -> memory::observer_ptr<const graphics::descriptor> {
//   if (auto it = _descriptors.find(name); it != _descriptors.end()) {
//     return it->second;
//   }

//   return nullptr;
// }

// auto render_stage::descriptors() const noexcept -> const std::map<std::string, memory::observer_ptr<const graphics::descriptor>>& {
//   return _descriptors;
// }

// auto render_stage::_update_subpass_attachment_counts(const graphics::attachment& attachment) -> void {
//   for (const auto& subpass : _subpass_bindings) {
//     if (auto bindings = subpass.color_attachments(); ranges::contains(bindings, attachment.binding())) {
//       _subpass_attachment_counts[subpass.binding()]++;
//       _subpass_attachments[subpass.binding()].push_back(attachment.binding());
//     }
//   }
// }

// auto render_stage::_create_attachment_descriptions(VkFormat depth_format, VkFormat surface_format) -> std::vector<VkAttachmentDescription> {
//   auto attachments = std::vector<VkAttachmentDescription>{};

//   for (const auto& attachment : _attachments) {
//     auto attachment_description = VkAttachmentDescription{};
//     attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
//     attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//     attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//     attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//     attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//     attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

//     switch (attachment.image_type()) {
//       case attachment::type::image: {
//         attachment_description.format = to_vk_enum<VkFormat>(attachment.format());
//         attachment_description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//         break;
//       }
//       case attachment::type::storage: {
//         attachment_description.format = to_vk_enum<VkFormat>(attachment.format());
//         attachment_description.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
//         break;
//       }
//       case attachment::type::depth: {
//         attachment_description.format = depth_format;
//         attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//         break;
//       }
//       case attachment::type::swapchain: {
//         attachment_description.format = surface_format;
//         attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//         break;
//       }
//     }

//     attachments.push_back(attachment_description);
//   }

//   return attachments;
// }

// auto render_stage::_create_subpass_dependencies() -> std::vector<VkSubpassDependency> {
//   auto subpass_dependencies = std::vector<VkSubpassDependency>{};

//   auto depth_dependency = VkSubpassDependency{};
//   depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//   depth_dependency.dstSubpass = 0;
//   depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;;
//   depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;;
//   depth_dependency.srcAccessMask = 0;
//   depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//   depth_dependency.dependencyFlags = 0;

//   subpass_dependencies.push_back(depth_dependency);

//   for (const auto i : std::views::iota(0u, _subpass_bindings.size() + 1u)) {
//     auto subpass_dependency = VkSubpassDependency{};

//     if (i == 0u) {
//       subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//       subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//       subpass_dependency.srcAccessMask = 0u;
		
//       subpass_dependency.dstSubpass = 0u;
//       subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//       subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			
//       subpass_dependency.dependencyFlags = 0u;
//     } else if (i == _subpass_bindings.size()) {
//       subpass_dependency.srcSubpass = i - 1u;
//       subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//       subpass_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

//       subpass_dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
//       subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//       subpass_dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

//       subpass_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//     } else {
//       subpass_dependency.srcSubpass = i - 1u;
//       subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//       subpass_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

//       subpass_dependency.dstSubpass = i;
//       subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//       subpass_dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

//       subpass_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//     }

// 		subpass_dependencies.emplace_back(subpass_dependency);
//   }

//   return subpass_dependencies;
// }

// } // namespace sbx::graphics
