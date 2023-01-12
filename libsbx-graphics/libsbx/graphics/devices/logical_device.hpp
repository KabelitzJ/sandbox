#ifndef LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>

namespace sbx::graphics {

class logical_device {

public:

  logical_device(const instance& instance, const physical_device& physical_device);

  ~logical_device();

  auto handle() const noexcept -> VkDevice;

  operator VkDevice() const noexcept; 

private:

  VkDevice _handle{};

}; // class logical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_

