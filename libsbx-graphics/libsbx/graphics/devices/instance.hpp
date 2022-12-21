#ifndef LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

#include <libsbx/core/target.hpp>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::graphics {

class instance : utility::noncopyable {

public:

  instance();

  ~instance();

  VkInstance handle() const noexcept;

  operator VkInstance() const noexcept;

private:

  std::vector<const char*> _extensions();

  std::vector<const char*> _layers();

#if defined(SBX_DEBUG)
  void _populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);

  VkResult _create_debug_messenger(VkInstance instance_handle, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger);

  void _destroy_debug_messenger(VkInstance instance_handle, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator);
#endif

#if defined(SBX_DEBUG)
  VkDebugUtilsMessengerEXT _debug_messenger{};
#endif

  VkInstance _handle{};

}; // class instance

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_
