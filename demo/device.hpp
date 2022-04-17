#ifndef DEMO_DEVICE_HPP_
#define DEMO_DEVICE_HPP_

#include <string>

#include <GLFW/glfw3.h>

#include "window.hpp"

namespace demo {

class device {

public:

  device(window& window, const std::string& name)
  : _window(window),
    _instance{VK_NULL_HANDLE} {
    _create_instance(name);
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
    auto app_info = VkApplicationInfo{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = name.c_str();
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Sandbox";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    auto create_info = VkInstanceCreateInfo{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = nullptr;

    if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create instance!");
    }
  }

  window& _window;
  VkInstance _instance{};

}; // class device

} // namespace demo

#endif // DEMO_DEVICE_HPP_
