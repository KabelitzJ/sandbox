#ifndef LIBSBX_GRAPHICS_DEVICES_PHYSICAL_DEVICE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_PHYSICAL_DEVICE_HPP_

#include <vector>
#include <cstring>

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/devices/instance.hpp>

namespace sbx::graphics {

class physical_device {

public:

  physical_device(const instance& instance);

  ~physical_device();

  auto handle() const noexcept -> VkPhysicalDevice;

  operator VkPhysicalDevice() const noexcept; 

private:

  auto _choose_device(const std::vector<VkPhysicalDevice>& devices) -> VkPhysicalDevice;

  VkPhysicalDevice _handle{};

}; // class physical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_PHYSICAL_DEVICE_HPP_

