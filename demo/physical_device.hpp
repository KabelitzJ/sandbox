#ifndef DEMO_PHYSICAL_DEVICE_HPP_
#define DEMO_PHYSICAL_DEVICE_HPP_

#include <vector>
#include <optional>

#include <vulkan/vulkan.hpp>

#include <types/primitives.hpp>
#include <platform/target.hpp>

#include "logger.hpp"
#include "instance.hpp"

namespace demo {

class physical_device {

  friend class logical_device;

public:

  physical_device(logger* logger, instance* instance)
  : _logger{logger},
    _instance{instance},
    _handle{nullptr},
    _queue_family_indices{} {
    _initialize();
  }

  physical_device(const physical_device&) = delete;

  physical_device(physical_device&&) = delete;

  ~physical_device() = default;

  physical_device& operator=(const physical_device&) = delete;

  physical_device& operator=(physical_device&&) = delete;

  [[nodiscard]] VkPhysicalDevice handle() const noexcept {
    return _handle;
  }

private:

  struct queue_family_indices {
    std::optional<sbx::uint32> graphics_family{};

    bool is_complete() const noexcept {
      return graphics_family.has_value();
    }
  };

  void _initialize() {
    auto available_device_count = sbx::uint32{0};
    auto available_devices = std::vector<VkPhysicalDevice>{};

    vkEnumeratePhysicalDevices(_instance->handle(), &available_device_count, nullptr);

    if (available_device_count == 0) {
      throw std::runtime_error("Failed to find any GPUs with Vulkan support");
    }

    available_devices.resize(available_device_count);

    vkEnumeratePhysicalDevices(_instance->handle(), &available_device_count, available_devices.data());

    for (auto& device : available_devices) {
      const auto queue_families = _find_queue_families(device);

      if (queue_families.is_complete()) {
        _handle = device;
        _queue_family_indices = queue_families;
        break;
      }
    }

    if (!_handle) {
      throw std::runtime_error("Failed to find a suitable GPU");
    }

#if defined(SBX_DEBUG)
    auto device_properties = VkPhysicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(_handle, &device_properties);

    auto device_features = VkPhysicalDeviceFeatures{};
    vkGetPhysicalDeviceFeatures(_handle, &device_features);

    _logger->info("Selected GPU: {}", device_properties.deviceName);
#endif
  }

  queue_family_indices _find_queue_families(VkPhysicalDevice device) {
    auto indices = queue_family_indices{};

    auto queue_family_count = sbx::uint32{0};
    auto queue_families = std::vector<VkQueueFamilyProperties>{};

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    queue_families.resize(queue_family_count);

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    auto i = sbx::uint32{0};

    for (const auto& queue_family : queue_families) {
      if (indices.is_complete()) {
        break;
      }

      if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphics_family = i;
      }

      ++i;
    }

    return indices;
  }

  logger* _logger{};
  instance* _instance{};

  VkPhysicalDevice _handle{};
  queue_family_indices _queue_family_indices{};

}; // physical_device

} // namespace demo

#endif // DEMO_PHYSICAL_DEVICE_HPP_
