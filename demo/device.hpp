#ifndef DEMO_DEVICE_HPP_
#define DEMO_DEVICE_HPP_

#include <string>

#include <GLFW/glfw3.h>

#include "validation_layers.hpp"
#include "configuration.hpp"
#include "window.hpp"
#include "logger.hpp"

namespace demo {

class device {

public:

  device(logger* logger, configuration* configuration, window* window)
  : _logger{logger},
    _configuration{configuration},
    _window{window},
    _instance{VK_NULL_HANDLE},
    _physical_device{VK_NULL_HANDLE},
    _logical_device{VK_NULL_HANDLE} {
    _create_instance();
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

  void _create_instance() {
    const auto app_info = VkApplicationInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = _configuration->get<std::string>("name").c_str(),
      .applicationVersion = VK_MAKE_VERSION(
        _configuration->get<sbx::uint32>("version.major"),
        _configuration->get<sbx::uint32>("version.minor"),
        _configuration->get<sbx::uint32>("version.patch")
      ),
      .pEngineName = "Sandbox Engine",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),
      .apiVersion = VK_API_VERSION_1_0
    };

    const auto create_info = VkInstanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = validation_layers::count(),
      .ppEnabledLayerNames = validation_layers::names(),
      .enabledExtensionCount = 0,
      .ppEnabledExtensionNames = nullptr
    };

    if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create vulkan instance!");
    }
  }

  void _create_physical_device() {

  }

  void _create_logical_device() {

  }

  logger* _logger{};
  configuration* _configuration{};
  window* _window{};

  VkInstance _instance{VK_NULL_HANDLE};
  VkPhysicalDevice _physical_device{VK_NULL_HANDLE};
  VkDevice _logical_device{VK_NULL_HANDLE};


}; // class device

} // namespace demo

#endif // DEMO_DEVICE_HPP_
