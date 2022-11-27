#ifndef LIBSBX_DEVICES_DEVICE_MODULE_HPP_
#define LIBSBX_DEVICES_DEVICE_MODULE_HPP_

#include <memory>
#include <vector>
#include <cstdint>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/platform.hpp>

#include <libsbx/devices/monitor.hpp>
#include <libsbx/devices/window.hpp>

namespace sbx::devices {

class device_module : public core::module<device_module> {

  inline static const auto registered = register_module(stage::normal);

public:

  device_module() {
    if (!glfwInit()) {
      throw std::runtime_error{"Failed to initialize GLFW"};
    }

    if (!glfwVulkanSupported()) {
      throw std::runtime_error{"Vulkan is not supported"};
    }

    _monitor = std::make_unique<monitor>();
    _window = std::make_unique<window>();
  }

  ~device_module() override {
    glfwTerminate();
  }

  void update() override {
    glfwPollEvents();
  }

  std::vector<const char*> required_extentions() {
    auto extension_count = std::uint32_t{0};

    const auto glfw_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

    auto extensions = std::vector<const char*>{glfw_extensions, glfw_extensions + extension_count};

#if defined(LIBSBX_DEBUG)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return extensions;
  }

  monitor& current_monitor() {
    return *_monitor;
  }

  window& current_window() {
    return *_window;
  }

private:

  std::unique_ptr<monitor> _monitor{};
  std::unique_ptr<window> _window{};

}; // class device_module

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_DEVICE_MODULE_HPP_
