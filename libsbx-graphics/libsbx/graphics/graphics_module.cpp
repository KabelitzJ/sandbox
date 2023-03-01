#include <libsbx/graphics/graphics_module.hpp>

#include <fmt/format.h>

#include <libsbx/utility/fast_mod.hpp>

#include <yaml-cpp/yaml.h>

namespace sbx::graphics {

static auto _stringify_result(VkResult result) -> std::string {
  switch (result) {
    case VK_SUCCESS:
      return "Success";
    case VK_NOT_READY:
      return "A fence or query has not yet completed";
    case VK_TIMEOUT:
      return "A wait operation has not completed in the specified time";
    case VK_EVENT_SET:
      return "An event is signaled";
    case VK_EVENT_RESET:
      return "An event is unsignaled";
    case VK_INCOMPLETE:
      return "A return array was too small for the result";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return "A host memory allocation has failed";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      return "A device memory allocation has failed";
    case VK_ERROR_INITIALIZATION_FAILED:
      return "Initialization of an object could not be completed for implementation-specific reasons";
    case VK_ERROR_DEVICE_LOST:
      return "The logical or physical device has been lost";
    case VK_ERROR_MEMORY_MAP_FAILED:
      return "Mapping of a memory object has failed";
    case VK_ERROR_LAYER_NOT_PRESENT:
      return "A requested layer is not present or could not be loaded";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      return "A requested extension is not supported";
    case VK_ERROR_FEATURE_NOT_PRESENT:
      return "A requested feature is not supported";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
    case VK_ERROR_TOO_MANY_OBJECTS:
      return "Too many objects of the type have already been created";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      return "A requested format is not supported on this device";
    case VK_ERROR_SURFACE_LOST_KHR:
      return "A surface is no longer available";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      return "A allocation failed due to having no more space in the descriptor pool";
    case VK_SUBOPTIMAL_KHR:
      return "A swapchain no longer matches the surface properties exactly, but can still be used";
    case VK_ERROR_OUT_OF_DATE_KHR:
      return "A surface has changed in such a way that it is no longer compatible with the swapchain";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
      return "The display used by a swapchain does not use the same presentable image layout";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
      return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
    case VK_ERROR_VALIDATION_FAILED_EXT:
      return "A validation layer found an error";
    default:
      return "Unknown Vulkan error";
  }
}

auto validate(VkResult result) -> void {
  if (result >= VK_SUCCESS) {
    return;
  }

  throw std::runtime_error{_stringify_result(result)};
}

graphics_module::graphics_module()
: _instance{std::make_unique<graphics::instance>()},
  _physical_device{std::make_unique<graphics::physical_device>(*_instance)},
  _logical_device{std::make_unique<graphics::logical_device>(*_physical_device)},
  _surface{std::make_unique<graphics::surface>(*_instance, *_physical_device, *_logical_device)} {

}

graphics_module::~graphics_module() {
  const auto& graphics_queue = _logical_device->graphics_queue();

  validate(vkQueueWaitIdle(graphics_queue));

  _pipelines.clear();

  _swapchain.reset();

  for (const auto& frame_data : _per_frame_data) {
    vkDestroyFence(*_logical_device, frame_data.in_flight_fence, nullptr);
    vkDestroySemaphore(*_logical_device, frame_data.render_finished_semaphore, nullptr);
    vkDestroySemaphore(*_logical_device, frame_data.image_available_semaphore, nullptr);
  }

  // // [NOTE] KAJ 2023-02-19 20:47 - Command buffers must be freed before the command pools
  _command_buffers.clear();
  _command_pools.clear();
}

auto graphics_module::initialize() -> void {
  auto& window = devices::devices_module::get().window();

  window.set_on_framebuffer_resized([this]([[maybe_unused]] const devices::framebuffer_resized_event& event) {
    _framebuffer_resized = true;
  });

  _render_pass = std::make_unique<graphics::render_pass>();

  _recreate_swapchain();

  _mesh = std::make_unique<graphics::mesh>("./demo/assets/meshes/square.yaml");
}

auto graphics_module::update([[maybe_unused]] std::float_t delta_time) -> void {
  const auto& window = devices::devices_module::get().window();

  if (window.is_iconified()) {
    return;
  }

  if (_framebuffer_resized) {
    _recreate_swapchain();
  }

  const auto& frame_data = _per_frame_data[_current_frame];

  // Get the next image in the swapchain (back/front buffer)
  const auto result = _swapchain->acquire_next_image(frame_data.image_available_semaphore, frame_data.in_flight_fence);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    _framebuffer_resized = true;
    return;
  }else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error{"Failed to acquire swapchain image"};
  }

  const auto& command_buffer = _command_buffers[_swapchain->active_image_index()];

  _start_render_pass();

  // // [NOTE] KAJ 2023-02-19 17:39 - Drawing happens here

  const auto& pipeline = _pipelines["basic"];
  command_buffer->bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

  command_buffer->bind_vertex_buffer(0, _mesh->vertex_buffer());
  command_buffer->bind_index_buffer(_mesh->index_buffer(), 0, VK_INDEX_TYPE_UINT32);

  command_buffer->draw_indexed(static_cast<std::uint32_t>(_mesh->index_buffer().size() / sizeof(std::uint32_t)), 1, 0, 0, 0);

  _end_render_pass();
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

auto graphics_module::command_pool(const std::thread::id& thread_id) -> const std::shared_ptr<graphics::command_pool>& {
  if (auto entry = _command_pools.find(thread_id); entry != _command_pools.end()) {
    return entry->second;
  }

  return _command_pools.insert({thread_id, std::make_shared<graphics::command_pool>(thread_id)}).first->second;
}

auto graphics_module::render_pass() -> graphics::render_pass& {
  return *_render_pass;
}

auto graphics_module::swapchain() -> graphics::swapchain& {
  return *_swapchain;
};

auto graphics_module::load_pipeline(const std::filesystem::path& path) -> graphics::pipeline& {
  const auto name = path.stem().string();

  if (auto entry = _pipelines.find(name); entry != _pipelines.end()) {
    return *entry->second;
  }

  return *_pipelines.insert({name, std::make_unique<graphics::pipeline>(path)}).first->second;
}

auto graphics_module::pipeline(const std::string& name) -> graphics::pipeline& {
  if (auto entry = _pipelines.find(name); entry != _pipelines.end()) {
    return *entry->second;
  }

  throw std::runtime_error{"Pipeline not found"};
}

auto graphics_module::_start_render_pass() -> void {
  const auto& command_buffer = _command_buffers[_swapchain->active_image_index()];

  if (!command_buffer->is_running()) {
    command_buffer->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
  }

  const auto render_area = VkRect2D{VkOffset2D{0, 0}, _swapchain->extent()};

  auto viewport = VkViewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(render_area.extent.width);
	viewport.height = static_cast<float>(render_area.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	command_buffer->set_viewport(viewport);

	auto scissor = VkRect2D{};
	scissor.offset = render_area.offset;
	scissor.extent = render_area.extent;
  
  command_buffer->set_scissor(scissor);

  const auto clear_values = std::array<VkClearValue, 2>{
    VkClearValue{.color = VkClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}},
    VkClearValue{.depthStencil = VkClearDepthStencilValue{1.0f, 0}}
  };

  auto render_pass_begin_info = VkRenderPassBeginInfo{};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = *_render_pass;
	render_pass_begin_info.framebuffer = _swapchain->current_framebuffer();
	render_pass_begin_info.renderArea = render_area;
	render_pass_begin_info.clearValueCount = static_cast<std::uint32_t>(clear_values.size());
	render_pass_begin_info.pClearValues = clear_values.data();

  command_buffer->begin_render_pass(render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

auto graphics_module::_end_render_pass() -> void {
  const auto& frame_data = _per_frame_data[_current_frame];

  const auto active_image_index = _swapchain->active_image_index();
  auto& command_buffer = _command_buffers[active_image_index];

  const auto& present_queue = _logical_device->present_queue();

  command_buffer->end_render_pass();

  // Submit the command buffer to the graphics queue and draw the on the image
  command_buffer->end();
  command_buffer->submit(frame_data.image_available_semaphore, frame_data.render_finished_semaphore, frame_data.in_flight_fence);

  // Present the image to the screen
  const auto result = _swapchain->queue_present(present_queue, frame_data.render_finished_semaphore);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    _framebuffer_resized = true;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error{"Failed to present swapchain image"};
  }

  _current_frame = utility::fast_mod(_current_frame + 1, _swapchain->image_count());
}

auto graphics_module::_recreate_swapchain() -> void {
  _logical_device->wait_idle();

  const auto& window = devices::devices_module::get().window();

  const auto extent = VkExtent2D{window.width(), window.height()};

  _swapchain = std::make_unique<graphics::swapchain>(extent, _swapchain);

  _recreate_command_buffers();

  _framebuffer_resized = false;
}

auto graphics_module::_recreate_command_buffers() -> void {
  const auto image_count = _swapchain->image_count();

  for (const auto& data : _per_frame_data) {
    vkDestroyFence(*_logical_device, data.in_flight_fence, nullptr);
    vkDestroySemaphore(*_logical_device, data.image_available_semaphore, nullptr);
    vkDestroySemaphore(*_logical_device, data.render_finished_semaphore, nullptr);
  }

  _per_frame_data.resize(image_count);

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

  _command_buffers.resize(image_count);

  for (auto& command_buffer : _command_buffers) {
    command_buffer = std::make_unique<graphics::command_buffer>(false);
  }
}

} // namespace sbx::graphics
