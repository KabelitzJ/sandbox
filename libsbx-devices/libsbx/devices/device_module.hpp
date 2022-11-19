#ifndef LIBSBX_DEVICES_DEVICE_MODULE_HPP_
#define LIBSBX_DEVICES_DEVICE_MODULE_HPP_

#include <memory>

#include <GLFW/glfw3.h>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/monitor.hpp>
#include <libsbx/devices/window.hpp>

namespace sbx::devices {

class device_module : public core::module<device_module> {

  inline static const auto registered = register_module(stage::normal);

public:

  device_module() {
    glfwSetErrorCallback([](std::int32_t error_code, const char* description){
      sbx::core::logger::error("({}) {}", error_code, description);
    });

    if (!glfwInit()) {
      throw std::runtime_error{"Failed to initialize GLFW"};
    }

    if (!glfwVulkanSupported()) {
      throw std::runtime_error{"Vulkan is not supported"};
    }

    _monitor = std::make_unique<monitor>();
    _window = std::make_unique<window>(window_create_info{"Window", 960, 720});
  }

  ~device_module() override {
    _window.reset();
    _monitor.reset();

    glfwTerminate();
  }

  void update() override {
    glfwPollEvents();
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
