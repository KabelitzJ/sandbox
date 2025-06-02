#include <libsbx/graphics/graphics_module.hpp>

#include <ranges>

#include <fmt/format.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <libsbx/utility/fast_mod.hpp>
#include <libsbx/utility/logger.hpp>

#include <libsbx/core/engine.hpp>

namespace sbx::graphics {

auto validate(VkResult result) -> void {
  if (result >= VK_SUCCESS) {
    return;
  }

  throw std::runtime_error{"Validation error"};
}

graphics_module::graphics_module()
: _instance{std::make_unique<graphics::instance>()},
  _physical_device{std::make_unique<graphics::physical_device>(*_instance)},
  _logical_device{std::make_unique<graphics::logical_device>(*_physical_device)},
  _surface{std::make_unique<graphics::surface>(*_instance, *_physical_device, *_logical_device)} {
  auto& devices_module = core::engine::get_module<devices::devices_module>();

  auto& window = devices_module.window();

  window.on_framebuffer_resized() += [this]([[maybe_unused]] const auto& event) {
    _is_framebuffer_resized = true;
  };

  auto vulkan_functions = VmaVulkanFunctions{};
  vulkan_functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  vulkan_functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

  auto allocator_info = VmaAllocatorCreateInfo{};
  allocator_info.physicalDevice = _physical_device->handle();
  allocator_info.device = _logical_device->handle();
  allocator_info.instance = _instance->handle();
  allocator_info.pVulkanFunctions = &vulkan_functions;
  allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

  vmaCreateAllocator(&allocator_info, &_allocator);
}

graphics_module::~graphics_module() {
  _logical_device->wait_idle();

  _renderer.reset();

  _swapchain.reset();

  for (const auto& frame_data : _per_frame_data) {
    vkDestroyFence(*_logical_device, frame_data.graphics_in_flight_fence, nullptr);
    vkDestroyFence(*_logical_device, frame_data.compute_in_flight_fence, nullptr);
    vkDestroySemaphore(*_logical_device, frame_data.image_available_semaphore, nullptr);
    vkDestroySemaphore(*_logical_device, frame_data.compute_finished_semaphore, nullptr);
  }

  for (const auto& image_data : _per_image_data) {
    vkDestroySemaphore(*_logical_device, image_data.render_finished_semaphore, nullptr);
  }

  // [NOTE] KAJ 2023-02-19 : Command buffers must be freed before the command pools
  _graphics_command_buffers.clear();
  _compute_command_buffers.clear();
  _command_pools.clear();

  for (const auto& [type, container] : _asset_containers) {
    container->clear();
  }

  _buffers.clear();
  _shaders.clear();
  _graphics_pipelines.clear();
  _compute_pipelines.clear();
  _images.clear();
  _cube_image.clear();

  vmaDestroyAllocator(_allocator);
}

auto graphics_module::update() -> void {
  auto& devices_module = core::engine::get_module<devices::devices_module>();

  const auto& window = devices_module.window();

  if (!_renderer || window.is_iconified()) {
    return;
  }

  const auto& frame_data = _per_frame_data[_current_frame];

  // [NOTE] KAJ 2023-02-19 : Compute happens here

  validate(vkWaitForFences(_logical_device->handle(), 1, &frame_data.compute_in_flight_fence, true, std::numeric_limits<std::uint64_t>::max()));

  auto& compute_command_buffer = _compute_command_buffers[_current_frame];

  compute_command_buffer.begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

  _renderer->execute_tasks(compute_command_buffer);

  compute_command_buffer.release_ownership(_release_ownership_data);

  compute_command_buffer.end();

  compute_command_buffer.submit({}, frame_data.compute_finished_semaphore, frame_data.compute_in_flight_fence);

  if (_is_framebuffer_resized || _swapchain->is_outdated(_surface->current_extent())) {
    _recreate_swapchain();
    return;
  }

  // Get the next image in the swapchain (back/front buffer)
  const auto result = _swapchain->acquire_next_image(frame_data.image_available_semaphore, frame_data.graphics_in_flight_fence);
  
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    _recreate_swapchain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error{"Failed to acquire swapchain image"};
  }

  // [NOTE] KAJ 2023-02-19 : Drawing happens here

  auto stage = pipeline::stage{};

  auto& command_buffer = _graphics_command_buffers[_current_frame];
  vkResetCommandBuffer(command_buffer, 0);

  command_buffer.begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

  command_buffer.acquire_ownership(_acquire_ownership_data);

  for (const auto& render_stage : _renderer->render_stages()) {
    _start_render_pass(*render_stage, command_buffer);

    const auto& subpasses = render_stage->subpasses();

    for (const auto& subpass : subpasses) {
      stage.subpass = subpass.binding();

      _renderer->render(stage, command_buffer);

      if (subpass.binding() != subpasses.back().binding()) {
        vkCmdNextSubpass(command_buffer, VK_SUBPASS_CONTENTS_INLINE);
      }
    }

    _end_render_pass(*render_stage, command_buffer);

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

auto graphics_module::_start_render_pass(graphics::render_stage& render_stage, graphics::command_buffer& command_buffer) -> void {
  if (!command_buffer.is_running()) {
    command_buffer.begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
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

	command_buffer.set_viewport(viewport);

	auto scissor = VkRect2D{};
	scissor.offset = render_area.offset;
	scissor.extent = render_area.extent;
  
  command_buffer.set_scissor(scissor);

  const auto& clear_values = render_stage.clear_values();

  auto render_pass_begin_info = VkRenderPassBeginInfo{};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = render_stage.render_pass();
	render_pass_begin_info.framebuffer = render_stage.framebuffer(_swapchain->active_image_index());
	render_pass_begin_info.renderArea = render_area;
	render_pass_begin_info.clearValueCount = static_cast<std::uint32_t>(clear_values.size());
	render_pass_begin_info.pClearValues = clear_values.data();

  command_buffer.begin_render_pass(render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

auto graphics_module::_end_render_pass(graphics::render_stage& render_stage, graphics::command_buffer& command_buffer) -> void {
  auto& frame_data = _per_frame_data[_current_frame];
  auto& image_data = _per_image_data[_swapchain->active_image_index()];

  command_buffer.end_render_pass();

  if (!render_stage.has_swapchain_attachment()) {
    return;
  }

  // Submit the command buffer to the graphics queue and draw the on the image
  command_buffer.end();

  auto wait_semaphores = std::vector<command_buffer::wait_data>{};
  wait_semaphores.push_back({frame_data.image_available_semaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT});
  wait_semaphores.push_back({frame_data.compute_finished_semaphore, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT});

  command_buffer.submit(wait_semaphores, image_data.render_finished_semaphore, frame_data.graphics_in_flight_fence);

  // Present the image to the screen
  const auto result = _swapchain->present(image_data.render_finished_semaphore);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _is_framebuffer_resized) {
    // _is_framebuffer_resized = false;
    _recreate_swapchain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to present swapchain image"};
  }

  _current_frame = utility::fast_mod(_current_frame + 1, swapchain::max_frames_in_flight);
}

auto graphics_module::_reset_render_stages() -> void {
  _recreate_swapchain();
}

auto graphics_module::_recreate_swapchain() -> void {
  auto& devices_module = core::engine::get_module<devices::devices_module>();

  _logical_device->wait_idle();

  _swapchain = std::make_unique<graphics::swapchain>(_swapchain);

  _recreate_per_frame_data();
  _recreate_per_image_data();
  _recreate_command_buffers();

  for (const auto& render_stage : _renderer->render_stages()) {
    render_stage->rebuild(*_swapchain);
  }

  _recreate_attachments();

  _current_frame = 0;
  _is_framebuffer_resized = false;
}

auto graphics_module::_recreate_per_frame_data() -> void {
  for (const auto& data : _per_frame_data) {
    vkDestroyFence(*_logical_device, data.graphics_in_flight_fence, nullptr);
    vkDestroyFence(*_logical_device, data.compute_in_flight_fence, nullptr);
    vkDestroySemaphore(*_logical_device, data.image_available_semaphore, nullptr);
    vkDestroySemaphore(*_logical_device, data.compute_finished_semaphore, nullptr);
  }

  _per_frame_data.resize(swapchain::max_frames_in_flight);

  auto semaphore_create_info = VkSemaphoreCreateInfo{};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	auto fence_create_info = VkFenceCreateInfo{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (auto& data : _per_frame_data) {
    validate(vkCreateSemaphore(*_logical_device, &semaphore_create_info, nullptr, &data.image_available_semaphore));
    validate(vkCreateSemaphore(*_logical_device, &semaphore_create_info, nullptr, &data.compute_finished_semaphore));
    validate(vkCreateFence(*_logical_device, &fence_create_info, nullptr, &data.graphics_in_flight_fence));
    validate(vkCreateFence(*_logical_device, &fence_create_info, nullptr, &data.compute_in_flight_fence));
  }
}

auto graphics_module::_recreate_per_image_data() -> void {
  for (auto& data : _per_image_data) {
    vkDestroySemaphore(*_logical_device, data.render_finished_semaphore, nullptr);
  }

  _per_image_data.resize(_swapchain->image_count());

  auto semaphore_create_info = VkSemaphoreCreateInfo{};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  for (auto& data : _per_image_data) {
    validate(vkCreateSemaphore(*_logical_device, &semaphore_create_info, nullptr, &data.render_finished_semaphore));
  }
}

auto graphics_module::_recreate_command_buffers() -> void {
  _graphics_command_buffers.clear();
  _graphics_command_buffers.reserve(swapchain::max_frames_in_flight);

  for (auto i : std::views::iota(0u, swapchain::max_frames_in_flight)) {
    _graphics_command_buffers.emplace_back(false, VK_QUEUE_GRAPHICS_BIT);
  }

  _compute_command_buffers.clear();
  _compute_command_buffers.reserve(swapchain::max_frames_in_flight);

  for (auto i : std::views::iota(0u, swapchain::max_frames_in_flight)) {
    _compute_command_buffers.emplace_back(false, VK_QUEUE_COMPUTE_BIT);
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
