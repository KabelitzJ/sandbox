#ifndef DEMO_LOGICAL_DEVICE_HPP_
#define DEMO_LOGICAL_DEVICE_HPP_

#include <unordered_set>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <utils/noncopyable.hpp>

#include "instance.hpp"
#include "physical_device.hpp"

namespace demo {

class logical_device : sbx::noncopyable {

public:

  logical_device(instance* instance, physical_device* physical_device)
  : _instance{instance},
    _physical_device{physical_device},
    _handle{nullptr},
    _graphics_queue{nullptr} {
    _initialize();
  }

  ~logical_device() {
    _terminate();
  }

  [[nodiscard]] VkDevice handle() const noexcept {
    return _handle;
  }

  [[nodiscard]] VkQueue graphics_queue() const noexcept {
    return _graphics_queue;
  }

  [[nodiscard]] VkQueue present_queue() const noexcept {
    return _present_queue;
  }

  void wait_till_idle() {
    vkDeviceWaitIdle(_handle);
  }

private:

  void _initialize() {
    const auto graphics_family = _physical_device->graphics_family();
    const auto present_family = _physical_device->present_family();

    auto unique_queue_family_indices = std::unordered_set<sbx::uint32>{
      graphics_family,
      present_family
    };

    const auto queue_priority = sbx::float32{1.0f};

    auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};

    for (const auto queue_family_index : unique_queue_family_indices) {
      auto queue_create_info = VkDeviceQueueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queue_family_index,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority
      };

      queue_create_infos.push_back(queue_create_info);
    }

    const auto layers = _instance->_layers();
    const auto extensions = _physical_device->_extensions();

    const auto features = VkPhysicalDeviceFeatures{};

    const auto device_create_info = VkDeviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCreateInfoCount = static_cast<sbx::uint32>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),
      .enabledLayerCount = static_cast<sbx::uint32>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<sbx::uint32>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
      .pEnabledFeatures = &features
    };

    if (vkCreateDevice(_physical_device->handle(), &device_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create logical device"};
    }

    vkGetDeviceQueue(_handle, graphics_family, 0, &_graphics_queue);
    vkGetDeviceQueue(_handle, present_family, 0, &_present_queue);
  }

  void _terminate() {
    vkDestroyDevice(_handle, nullptr);
  }

  instance* _instance{};
  physical_device* _physical_device{};

  VkDevice _handle{};
  VkQueue _graphics_queue{};
  VkQueue _present_queue{};

}; // logical_device

} // namespace demo

#endif // DEMO_LOGICAL_DEVICE_HPP_

