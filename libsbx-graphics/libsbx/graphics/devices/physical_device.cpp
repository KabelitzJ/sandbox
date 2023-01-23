#include <libsbx/graphics/devices/physical_device.hpp>

#include <vector>
#include <cstring>
#include <array>
#include <map>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/devices/extensions.hpp>

namespace sbx::graphics {

static constexpr auto stage_flag_bits_v = std::array<VkSampleCountFlagBits, 6>{
	VK_SAMPLE_COUNT_64_BIT, 
  VK_SAMPLE_COUNT_32_BIT, 
  VK_SAMPLE_COUNT_16_BIT, 
  VK_SAMPLE_COUNT_8_BIT,
	VK_SAMPLE_COUNT_4_BIT, 
  VK_SAMPLE_COUNT_2_BIT
};

physical_device::physical_device(const instance& instance)
: _msaa_samples{VK_SAMPLE_COUNT_1_BIT} {
  auto physical_device_count = std::uint32_t{0};
  vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);

  auto physical_devices = std::vector<VkPhysicalDevice>{physical_device_count};
  vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data());

  _handle = _choose_device(physical_devices);

  if (!_handle) {
    throw std::runtime_error{"Could not find suitable GPU"};
  }

  vkGetPhysicalDeviceProperties(_handle, &_properties);
	vkGetPhysicalDeviceFeatures(_handle, &_features);
	vkGetPhysicalDeviceMemoryProperties(_handle, &_memory_properties);
	_msaa_samples = _get_max_usable_sample_count();

  core::logger::debug("Selected GPU '{}'", _properties.deviceName);
}

physical_device::~physical_device() {

}

auto physical_device::handle() const noexcept -> VkPhysicalDevice {
  return _handle;
}

physical_device::operator VkPhysicalDevice() const noexcept {
  return _handle;
}

auto physical_device::properties() const -> const VkPhysicalDeviceProperties& {
  return _properties;
}

auto physical_device::features() const -> const VkPhysicalDeviceFeatures& {
  return _features;
}

auto physical_device::memory_properties() const -> const VkPhysicalDeviceMemoryProperties& {
  return _memory_properties;
}

auto physical_device::msaa_samples() const -> const VkSampleCountFlagBits& {
  return _msaa_samples;
}

auto physical_device::_choose_device(const std::vector<VkPhysicalDevice>& devices) -> VkPhysicalDevice {
  auto scores = std::multimap<std::uint32_t, VkPhysicalDevice>{};

  for (const auto& device : devices) {
    const auto score = _score_device(device);
    scores.insert({score, device});
  }

  if (auto entry = scores.rbegin(); entry->first > 0) {
    return entry->second;
  }

  return nullptr;
}

auto physical_device::_score_device(const VkPhysicalDevice& device) -> std::uint32_t {
  auto score = std::uint32_t{};

  auto extension_property_count = std::uint32_t{0};
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_property_count, nullptr);

	auto extension_properties = std::vector<VkExtensionProperties>{extension_property_count};
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_property_count, extension_properties.data());

  for (const auto* current_extension : extensions::device()) {
    auto extension_found = false;

    for (const auto& extension : extension_properties) {
      if (std::strcmp(current_extension, extension.extensionName) == 0) {
        extension_found = true;
        break;
      }
    }

    if (!extension_found) {
      return 0;
    }
  }

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

auto physical_device::_get_max_usable_sample_count() const -> VkSampleCountFlagBits {
  auto physical_device_properties = VkPhysicalDeviceProperties{};
	vkGetPhysicalDeviceProperties(_handle, &physical_device_properties);

	const auto counts = std::min(physical_device_properties.limits.framebufferColorSampleCounts, physical_device_properties.limits.framebufferDepthSampleCounts);

	for (const auto& sample_flag : stage_flag_bits_v) {
		if (counts & sample_flag) {
			return sample_flag;
    }
	}

	return VK_SAMPLE_COUNT_1_BIT;
}

} // namespace sbx::graphics
