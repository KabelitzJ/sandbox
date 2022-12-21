#ifndef LIBSBX_GRAPHICS_PHYSICAL_DEVICE_HPP_
#define LIBSBX_GRAPHICS_PHYSICAL_DEVICE_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/graphics/devices/instance.hpp>

namespace sbx::graphics {

class physical_device : utility::noncopyable {

public:

  physical_device(const instance& instance);

  ~physical_device();

  VkPhysicalDevice handle() const noexcept;

  operator VkPhysicalDevice() const noexcept;

  VkPhysicalDeviceProperties properties() const noexcept;

  VkPhysicalDeviceFeatures features() const noexcept;

  VkPhysicalDeviceMemoryProperties memory_properties() const noexcept;

  VkSampleCountFlagBits sample_count() const noexcept;

private:

  VkPhysicalDevice _choose_device(const std::vector<VkPhysicalDevice>& devices);
  std::uint32_t _score_device(const VkPhysicalDevice& device);
  VkSampleCountFlagBits _max_usable_sample_count();

  VkPhysicalDevice _handle{};
  VkPhysicalDeviceProperties _properties{};
  VkPhysicalDeviceFeatures _features{};
  VkPhysicalDeviceMemoryProperties _memory_properties{};
  VkSampleCountFlagBits _sample_count{};

}; // class physical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PHYSICAL_DEVICE_HPP_
