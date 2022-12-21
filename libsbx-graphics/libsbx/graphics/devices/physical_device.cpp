#include <libsbx/graphics/devices/physical_device.hpp>

#include <vector>
#include <map>
#include <cinttypes>
#include <iomanip>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

physical_device::physical_device(const instance& instance)
: _sample_count{VK_SAMPLE_COUNT_1_BIT} {
  auto physical_device_count = std::uint32_t{0};
	vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
	auto physical_devices = std::vector<VkPhysicalDevice>{physical_device_count};
	vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data());

  _handle = _choose_device(physical_devices);

  if (!_handle) {
    throw std::runtime_error{"Failed to find suitable gpu"};
  }

  vkGetPhysicalDeviceProperties(_handle, &_properties);
	vkGetPhysicalDeviceFeatures(_handle, &_features);
	vkGetPhysicalDeviceMemoryProperties(_handle, &_memory_properties);
  _sample_count = _max_usable_sample_count();

  core::logger::debug("Selected physical device: {}", _properties.deviceName);
}

physical_device::~physical_device() {

}

VkPhysicalDevice physical_device::handle() const noexcept {
  return _handle;
}

physical_device::operator VkPhysicalDevice() const noexcept {
  return _handle;
}

VkPhysicalDeviceProperties physical_device::properties() const noexcept {
  return _properties;
}

VkPhysicalDeviceFeatures physical_device::features() const noexcept {
  return _features;
}

VkPhysicalDeviceMemoryProperties physical_device::memory_properties() const noexcept {
  return _memory_properties;
}

VkSampleCountFlagBits physical_device::sample_count() const noexcept {
  return _sample_count;
}

VkPhysicalDevice physical_device::_choose_device(const std::vector<VkPhysicalDevice>& devices) {
  auto ranked_devices = std::multimap<std::uint32_t, VkPhysicalDevice>{};

  for (const auto& device : devices) {
    ranked_devices.insert({_score_device(device), device});
  }

  if (const auto entry = ranked_devices.begin(); entry->first > 0) {
    return entry->second;
  }

  return nullptr;
}

std::uint32_t physical_device::_score_device(const VkPhysicalDevice& device) {
  auto score = std::uint32_t{0};

  auto extension_property_count = std::uint32_t{0};
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_property_count, nullptr);
	auto extension_properties = std::vector<VkExtensionProperties>{extension_property_count};
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_property_count, extension_properties.data());

  auto physical_device_properties = VkPhysicalDeviceProperties{};
	vkGetPhysicalDeviceProperties(device, &physical_device_properties);

	auto physical_device_features = VkPhysicalDeviceFeatures{};
	vkGetPhysicalDeviceFeatures(device, &physical_device_features);

  if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  score += physical_device_properties.limits.maxImageDimension2D;

  return score;
}

VkSampleCountFlagBits physical_device::_max_usable_sample_count() {
  const auto stage_flag_bits = std::vector<VkSampleCountFlagBits>{
    VK_SAMPLE_COUNT_64_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_16_BIT, VK_SAMPLE_COUNT_8_BIT,
    VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_2_BIT
  };

  auto physical_device_properties = VkPhysicalDeviceProperties{};
	vkGetPhysicalDeviceProperties(_handle, &physical_device_properties);

  auto counts = std::min(physical_device_properties.limits.framebufferColorSampleCounts, physical_device_properties.limits.framebufferDepthSampleCounts);

  for (const auto& sample_flag : stage_flag_bits) {
		if (counts & sample_flag) {
			return sample_flag;
    }
	}

  return VK_SAMPLE_COUNT_1_BIT;
}

} // namespace sbx::graphics
