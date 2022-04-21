#ifndef DEMO_DEVICE_HPP_
#define DEMO_DEVICE_HPP_

#include <string>

#include <GLFW/glfw3.h>

#include "window.hpp"
#include "logger.hpp"

namespace demo {

class device {

public:

  device(window& window, const std::string& name, logger* logger)
  : _window(window),
    _instance{VK_NULL_HANDLE},
    _physical_device{VK_NULL_HANDLE},
    _logical_device{VK_NULL_HANDLE},
    _logger{logger} {
    _create_instance(name);
    _create_physical_device();
    _create_logical_device();
  }

  device(const device& other) = delete;

  device(device&& other) noexcept;

  ~device() {
    vkDestroyInstance(_instance, nullptr);
  }

  device& operator=(const device& other) = delete;

  device& operator=(device&& other) noexcept;

private:

  void _create_instance(const std::string& name) {
    const auto app_info = VkApplicationInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = name.c_str(),
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0
    };

    const auto create_info = VkInstanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
      .enabledExtensionCount = 0,
      .ppEnabledExtensionNames = nullptr,
    };

    if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create vulkan instance!");
    }
  }

  void _create_physical_device() {

  }

  void _create_logical_device() {

  }

  window& _window;
  VkInstance _instance{VK_NULL_HANDLE};
  VkPhysicalDevice _physical_device{VK_NULL_HANDLE};
  VkDevice _logical_device{VK_NULL_HANDLE};

  logger* _logger{};

}; // class device

} // namespace demo

#endif // DEMO_DEVICE_HPP_
