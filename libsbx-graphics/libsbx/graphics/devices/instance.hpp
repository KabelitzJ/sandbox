#ifndef LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/core/platform.hpp>

namespace sbx::graphics {

class instance {

public:

  instance();

  ~instance();

  auto handle() const noexcept -> VkInstance;

  operator VkInstance() const noexcept;

private:

#if defined(SBX_DEBUG)

  auto _populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info) -> void;

  auto _create_debug_messenger(VkInstance instance_handle, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger) -> VkResult;

  auto _destroy_debug_messenger(VkInstance instance_handle, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator) -> void;

  VkDebugUtilsMessengerEXT _debug_messenger{};

#endif

  VkInstance _handle{};

}; // class instance

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_
