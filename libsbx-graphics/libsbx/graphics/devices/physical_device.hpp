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

  auto handle() const noexcept -> const VkPhysicalDevice&;

  operator const VkPhysicalDevice&() const noexcept;

	auto properties() const -> const VkPhysicalDeviceProperties&;

	auto features() const -> const VkPhysicalDeviceFeatures&;

	auto memory_properties() const -> const VkPhysicalDeviceMemoryProperties&;

	auto msaa_samples() const -> const VkSampleCountFlagBits&;

private:

  auto _choose_device(const std::vector<VkPhysicalDevice>& devices) -> VkPhysicalDevice;

  auto _score_device(const VkPhysicalDevice& device) -> std::uint32_t;

  auto _get_max_usable_sample_count() const -> VkSampleCountFlagBits;

  VkPhysicalDevice _handle{};
  VkPhysicalDeviceProperties _properties{};
	VkPhysicalDeviceFeatures _features{};
	VkPhysicalDeviceMemoryProperties _memory_properties{};
  VkSampleCountFlagBits _msaa_samples{};

}; // class physical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_PHYSICAL_DEVICE_HPP_

