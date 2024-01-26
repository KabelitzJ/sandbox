#include <libsbx/graphics/graphics_module.hpp>

#include <fmt/format.h>

#include <vulkan/vk_enum_string_helper.h>

#include <libsbx/utility/fast_mod.hpp>

#include <libsbx/core/engine.hpp>
#include <libsbx/core/logger.hpp>

namespace sbx::graphics {

auto validate(VkResult result) -> void {
  if (result >= VK_SUCCESS) {
    return;
  }

  throw std::runtime_error{string_VkResult(result)};
}

graphics_module::graphics_module()
: _instance{std::make_unique<graphics::instance>()},
  _physical_device{std::make_unique<graphics::physical_device>(*_instance)},
  _logical_device{std::make_unique<graphics::logical_device>(*_physical_device)},
  _surface{std::make_unique<graphics::surface>(*_instance, *_physical_device, *_logical_device)} {
  auto& devices_module = core::engine::get_module<devices::devices_module>();

  auto& window = devices_module.window();

  window.on_framebuffer_resized() += [this]([[maybe_unused]] const auto& event) {
    _framebuffer_resized = true;
  };
}

graphics_module::~graphics_module() {
  _logical_device->wait_idle();

  const auto& graphics_queue = _logical_device->graphics_queue();
  validate(vkQueueWaitIdle(graphics_queue));

  const auto& transfer_queue = _logical_device->transfer_queue();
  validate(vkQueueWaitIdle(transfer_queue));

  const auto& compute_queue = _logical_device->compute_queue();
  validate(vkQueueWaitIdle(compute_queue));

  _renderer.reset();

  _swapchain.reset();

  for (const auto& frame_data : _per_frame_data) {
    vkDestroyFence(*_logical_device, frame_data.in_flight_fence, nullptr);
    vkDestroySemaphore(*_logical_device, frame_data.render_finished_semaphore, nullptr);
    vkDestroySemaphore(*_logical_device, frame_data.image_available_semaphore, nullptr);
  }

  _free_deletion_queue();

  // [NOTE] KAJ 2023-02-19 : Command buffers must be freed before the command pools
  _command_buffers.clear();
  _command_pools.clear();
}

auto graphics_module::update() -> void {
  auto& devices_module = core::engine::get_module<devices::devices_module>();

  const auto& window = devices_module.window();

  if (!_renderer || window.is_iconified()) {
    return;
  }

  const auto& frame_data = _per_frame_data[_current_frame];

  // Get the next image in the swapchain (back/front buffer)
  const auto result = _swapchain->acquire_next_image(frame_data.image_available_semaphore, frame_data.in_flight_fence);
  
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    _recreate_swapchain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error{"Failed to acquire swapchain image"};
  }

  // [NOTE] KAJ 2023-02-19 : Drawing happens here

  auto stage = pipeline::stage{};

  for (const auto& render_stage : _renderer->render_stages()) {
    render_stage->update();

    if (!_start_render_pass(*render_stage)) {
      return;
    }

    auto& command_buffer = _command_buffers[_current_frame];

    const auto& subpasses = render_stage->subpasses();

    for (const auto& subpass : subpasses) {
      stage.subpass = subpass.binding();

      _renderer->render(stage, *command_buffer);

      if (subpass.binding() != subpasses.back().binding()) {
        vkCmdNextSubpass(*command_buffer, VK_SUBPASS_CONTENTS_INLINE);
      }
    }

    _end_render_pass(*render_stage);

    stage.renderpass++;
  }
}

auto graphics_module::instance() -> graphics::instance&  {
  return *_instance;
}

auto graphics_module::physical_device() -> graphics::physical_device& {
  return *_physical_device;
}

auto graphics_module::logical_device() -> graphics::logical_device& {
  return *_logical_device;
}

auto graphics_module::surface() -> graphics::surface& {
  return *_surface;
}

auto graphics_module::command_pool(VkQueueFlagBits queue_type, const std::thread::id& thread_id) -> const std::shared_ptr<graphics::command_pool>& {
  const auto key = command_pool_key{queue_type, thread_id};

  if (auto entry = _command_pools.find(key); entry != _command_pools.end()) {
    return entry->second;
  }

  return _command_pools.insert({key, std::make_shared<graphics::command_pool>(queue_type)}).first->second;
}

auto graphics_module::swapchain() -> graphics::swapchain& {
  return *_swapchain;
};

auto graphics_module::render_stage(const pipeline::stage& stage) -> graphics::render_stage& {
  if (!_renderer) {
    throw std::runtime_error{"No renderer set"};
  }

  return _renderer->render_stage(stage);
}

auto graphics_module::attachment(const std::string& name) const -> const descriptor& {
  if (!_renderer) {
    throw std::runtime_error{"No renderer set"};
  }

  if (auto entry = _attachments.find(name); entry != _attachments.end()) {
    return *entry->second;
  }

  throw std::runtime_error{fmt::format("No attachment with name '{}' found", name)};
}

auto graphics_module::_start_render_pass(graphics::render_stage& render_stage) -> bool {
  if (render_stage.is_outdated()) {
    _recreate_pass(render_stage);
    return false;
  }

  const auto& command_buffer = _command_buffers[_current_frame];

  if (!command_buffer->is_running()) {
    command_buffer->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
  }

  const auto& area = render_stage.render_area();

  const auto& offset = area.offset();
  const auto& extent = area.extent();

  auto render_area = VkRect2D{};
  render_area.offset = VkOffset2D{offset.x(), offset.y()};
  render_area.extent = VkExtent2D{extent.x(), extent.y()};

  auto viewport = VkViewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<std::float_t>(render_area.extent.width);
	viewport.height = static_cast<std::float_t>(render_area.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	command_buffer->set_viewport(viewport);

	auto scissor = VkRect2D{};
	scissor.offset = render_area.offset;
	scissor.extent = render_area.extent;
  
  command_buffer->set_scissor(scissor);

  const auto& clear_values = render_stage.clear_values();

  auto render_pass_begin_info = VkRenderPassBeginInfo{};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = render_stage.render_pass();
	render_pass_begin_info.framebuffer = render_stage.framebuffer(_swapchain->active_image_index());
	render_pass_begin_info.renderArea = render_area;
	render_pass_begin_info.clearValueCount = static_cast<std::uint32_t>(clear_values.size());
	render_pass_begin_info.pClearValues = clear_values.data();

  command_buffer->begin_render_pass(render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

  return true;
}

auto graphics_module::_end_render_pass(graphics::render_stage& render_stage) -> void {
  const auto& frame_data = _per_frame_data[_current_frame];

  auto& command_buffer = _command_buffers[_current_frame];

  command_buffer->end_render_pass();

  if (!render_stage.has_swapchain_attachment()) {
    return;
  }

  // Submit the command buffer to the graphics queue and draw the on the image
  command_buffer->end();
  command_buffer->submit(frame_data.image_available_semaphore, frame_data.render_finished_semaphore, frame_data.in_flight_fence);

  // Present the image to the screen
  const auto result = _swapchain->present(frame_data.render_finished_semaphore);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    _framebuffer_resized = true;
    _recreate_swapchain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to present swapchain image"};
  }

  _current_frame = utility::fast_mod(_current_frame + 1, swapchain::max_frames_in_flight);
}

auto graphics_module::_reset_render_stages() -> void {
  _recreate_swapchain();

  for (auto& stage : _renderer->render_stages()) {
    stage->rebuild(*_swapchain);
  }

  _recreate_attachments();
}

auto  graphics_module::_recreate_pass(graphics::render_stage& render_stage) -> void {
  const auto& graphics_queue = _logical_device->graphics_queue();

  validate(vkQueueWaitIdle(graphics_queue));

  if (render_stage.has_swapchain_attachment() && _framebuffer_resized) {
    _recreate_swapchain();
  }

  render_stage.rebuild(*_swapchain);

  _recreate_attachments();
}

auto graphics_module::_recreate_swapchain() -> void {
  auto& devices_module = core::engine::get_module<devices::devices_module>();

  _logical_device->wait_idle();

  const auto& window = devices_module.window();

  const auto extent = VkExtent2D{window.width(), window.height()};

  _swapchain = std::make_unique<graphics::swapchain>(extent, _swapchain);

  _recreate_command_buffers();

  _framebuffer_resized = false;
}

auto graphics_module::_recreate_command_buffers() -> void {
  for (const auto& data : _per_frame_data) {
    vkDestroyFence(*_logical_device, data.in_flight_fence, nullptr);
    vkDestroySemaphore(*_logical_device, data.image_available_semaphore, nullptr);
    vkDestroySemaphore(*_logical_device, data.render_finished_semaphore, nullptr);
  }

  _per_frame_data.resize(swapchain::max_frames_in_flight);

  auto semaphore_create_info = VkSemaphoreCreateInfo{};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	auto fence_create_info = VkFenceCreateInfo{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (auto& data : _per_frame_data) {
    validate(vkCreateSemaphore(*_logical_device, &semaphore_create_info, nullptr, &data.image_available_semaphore));
    validate(vkCreateSemaphore(*_logical_device, &semaphore_create_info, nullptr, &data.render_finished_semaphore));
    validate(vkCreateFence(*_logical_device, &fence_create_info, nullptr, &data.in_flight_fence));
  }

  _command_buffers.resize(swapchain::max_frames_in_flight);

  for (auto& command_buffer : _command_buffers) {
    command_buffer = std::make_unique<graphics::command_buffer>(false);
  }
}

auto graphics_module::_recreate_attachments() -> void {
  _attachments.clear();

  for (const auto& render_stage : _renderer->render_stages()) {
    const auto& descriptors = render_stage->descriptors();
    _attachments.insert(descriptors.begin(), descriptors.end());
  }
}

} // namespace sbx::graphics
