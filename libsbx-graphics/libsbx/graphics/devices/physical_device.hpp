#ifndef LIBSBX_GRAPHICS_PHYSICAL_DEVICE_HPP_
#define LIBSBX_GRAPHICS_PHYSICAL_DEVICE_HPP_

#include <vulkan/vulkan.hpp>

namespace sbx::graphics {

class physical_device {

public:

  physical_device();

  ~physical_device();

  VkPhysicalDevice handle() const noexcept;

  operator VkPhysicalDevice() const noexcept;

private:

  VkPhysicalDevice _handle{};

}; // class physical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PHYSICAL_DEVICE_HPP_
