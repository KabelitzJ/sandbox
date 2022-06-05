#ifndef DEMO_LOGICAL_DEVICE_HPP_
#define DEMO_LOGICAL_DEVICE_HPP_

#include <vulkan/vulkan.hpp>

#include "physical_device.hpp"

namespace demo {

class logical_device {

public:

  logical_device(physical_device* physical_device)
  : _physical_device{physical_device},
    _handle{nullptr},
    _graphics_queue{nullptr} {
    _initialize();
  }

  logical_device(const logical_device&) = delete;

  logical_device(logical_device&&) = delete;

  ~logical_device() {
    _terminate();
  }

  logical_device& operator=(const logical_device&) = delete;

  logical_device& operator=(logical_device&&) = delete;

  [[nodiscard]] VkDevice handle() const noexcept {
    return _handle;
  }

private:

  void _initialize() {
    const auto queue_family_indices = _physical_device->_queue_family_indices;

    auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};

    const auto queue_priority = sbx::float32{1.0f};

    auto graphics_queue_create_info = VkDeviceQueueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = queue_family_indices.graphics_family.value(),
      .queueCount = 1,
      .pQueuePriorities = &queue_priority
    };

    queue_create_infos.push_back(graphics_queue_create_info);

    const auto extensions = std::vector<const char*>{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    const auto features = VkPhysicalDeviceFeatures{};

    const auto device_create_info = VkDeviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCreateInfoCount = static_cast<sbx::uint32>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
      .enabledExtensionCount = static_cast<sbx::uint32>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
      .pEnabledFeatures = &features
    };

    if (vkCreateDevice(_physical_device->handle(), &device_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create logical device"};
    }

    vkGetDeviceQueue(_handle, queue_family_indices.graphics_family.value(), 0, &_graphics_queue);
  }

  void _terminate() {
    vkDestroyDevice(_handle, nullptr);
  }

  physical_device* _physical_device{};

  VkDevice _handle{};
  VkQueue _graphics_queue{};

}; // logical_device

} // namespace demo

#endif // DEMO_LOGICAL_DEVICE_HPP_

