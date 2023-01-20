#ifndef LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
#define LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_

#include <memory>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>

namespace sbx::graphics {

class graphics_module : public core::module<graphics_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<devices::devices_module>{});

public:

  graphics_module()
  : _instance{std::make_unique<graphics::instance>()},
    _physical_device{std::make_unique<graphics::physical_device>(*_instance)},
    _logical_device{std::make_unique<graphics::logical_device>(*_instance, *_physical_device)} {
    auto& window = devices::devices_module::get().window();

    window.handle();
  }

  ~graphics_module() override {

  }

  auto update([[maybe_unused]] std::float_t delta_time) -> void override {

  }

  static auto validate(VkResult result) -> void {
    if (result >= 0) {
      return;
    }

    throw std::runtime_error{_stringify_result(result)};
  }

  instance& instance() {
    return *_instance;
  }

  physical_device& physical_device() {
    return *_physical_device;
  }

  logical_device& logical_device() {
    return *_logical_device;
  }
  
private:

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

  std::unique_ptr<graphics::instance> _instance{};
  std::unique_ptr<graphics::physical_device> _physical_device{};
  std::unique_ptr<graphics::logical_device> _logical_device{};

}; // class graphics_module

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
