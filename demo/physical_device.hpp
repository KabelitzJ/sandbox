#ifndef DEMO_PHYSICAL_DEVICE_HPP_
#define DEMO_PHYSICAL_DEVICE_HPP_

#include <vector>
#include <optional>
#include <string>

#include <vulkan/vulkan.hpp>

#include <types/primitives.hpp>
#include <platform/target.hpp>

#include "logger.hpp"
#include "instance.hpp"
#include "surface.hpp"

namespace demo {

class physical_device {

  friend class logical_device;

public:

  physical_device(logger* logger, instance* instance, surface* surface)
  : _logger{logger},
    _instance{instance},
    _surface{surface},
    _swapchain_support{},
    _queue_family_indices{},
    _handle{nullptr} {
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
    std::optional<sbx::uint32> present_family{};

    bool is_complete() const noexcept {
      return graphics_family.has_value() && present_family.has_value();
    }
  };

  struct swapchain_support_details {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats{};
    std::vector<VkPresentModeKHR> present_modes{};
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
      const auto has_extentions_support = _has_device_extentions_support(device);

      // Device does not support the required extensions 
      if (!has_extentions_support) {
        continue;
      }

      const auto swapchain_support = _query_swapchain_support(device);

      // Swapchain is not adequate
      if (swapchain_support.formats.empty() || swapchain_support.present_modes.empty()) {
        continue;
      }

      const auto queue_families = _find_queue_families(device);

      if (queue_families.is_complete()) {
        _swapchain_support = swapchain_support;
        _queue_family_indices = queue_families;
        _handle = device;
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

  bool _has_device_extentions_support(VkPhysicalDevice device) {
    auto available_extention_count = sbx::uint32{0};
    auto available_extensions = std::vector<VkExtensionProperties>{};

    vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extention_count, nullptr);

    available_extensions.resize(available_extention_count);

    vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extention_count, available_extensions.data());

    const auto required_extensions = _extensions();

    for (const auto* required_extension : required_extensions) {
      bool found = false;

      for (const auto& available_extension : available_extensions) {
        if (std::strcmp(required_extension, available_extension.extensionName) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        return false;
      }
    }

    return true;
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

      auto present_support = sbx::uint32{0};
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface->handle(), &present_support);

      if (present_support) {
        indices.present_family = i;
      }

      if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphics_family = i;
      }

      ++i;
    }

    return indices;
  }

  std::vector<const char*> _extensions() {
    return std::vector<const char*>{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
  }

  swapchain_support_details _query_swapchain_support(VkPhysicalDevice device) {
    auto details = swapchain_support_details{};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface->handle(), &details.capabilities);

    auto format_count = sbx::uint32{0};

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface->handle(), &format_count, nullptr);

    if (format_count != 0) {
      details.formats.resize(format_count);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface->handle(), &format_count, details.formats.data());
    }

    auto present_mode_count = sbx::uint32{0};

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface->handle(), &present_mode_count, nullptr);

    if (present_mode_count != 0) {
      details.present_modes.resize(present_mode_count);
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface->handle(), &present_mode_count, details.present_modes.data());
    }

    return details;
  }

  logger* _logger{};
  instance* _instance{};
  surface* _surface{};

  swapchain_support_details _swapchain_support{};
  queue_family_indices _queue_family_indices{};
  VkPhysicalDevice _handle{};

}; // physical_device

} // namespace demo

#endif // DEMO_PHYSICAL_DEVICE_HPP_
