#ifndef LIBSBX_DEVICES_DEVICES_MODULE_HPP_
#define LIBSBX_DEVICES_DEVICES_MODULE_HPP_

#include <memory>
#include <vector>
#include <cinttypes>

#include <GLFW/glfw3.h>

#include <libsbx/core/module.hpp>
#include <libsbx/core/logger.hpp>

#include <libsbx/devices/window.hpp>

namespace sbx::devices {

class devices_module : public core::module<devices_module> {

  inline static const auto is_registered = register_module(stage::normal);

public:

  devices_module() {
    if (!glfwInit()) {
      throw std::runtime_error{"Could not initialize glfw"};
    }

    if (!glfwVulkanSupported()) {
      throw std::runtime_error{"Glfw does not support vulkan"};
    }

    _window = std::make_unique<devices::window>("Demo", 960, 720);
  }

  ~devices_module() override {
    _window = nullptr;

    glfwTerminate();
  }

  auto initialize() -> void override {
    
  }

  auto update([[maybe_unused]] std::float_t delta_time) -> void override {
    glfwPollEvents();
  }

  window& window() {
    return *_window;
  }

  auto required_instance_extensions() const -> std::vector<const char*> {
    auto extension_count = std::uint32_t{0};
    auto extensions = glfwGetRequiredInstanceExtensions(&extension_count);

    return std::vector<const char*>{extensions, extensions + extension_count};
  }

private:

  std::unique_ptr<devices::window> _window{};

}; // class devices_module

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_DEVICES_MODULE_HPP_
