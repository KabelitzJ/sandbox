#ifndef LIBSBX_DEVICES_DEVICES_MODULE_HPP_
#define LIBSBX_DEVICES_DEVICES_MODULE_HPP_

#include <memory>
#include <vector>
#include <cinttypes>

#include <fmt/format.h>

#include <GLFW/glfw3.h>

#include <libsbx/core/module.hpp>
#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/units/time.hpp>

#include <libsbx/devices/window.hpp>

namespace sbx::devices {

class devices_module final : public core::module<devices_module> {

  inline static const auto is_registered = register_module(stage::normal);

public:

  devices_module() {
    if (!glfwInit()) {
      throw std::runtime_error{"Could not initialize glfw"};
    }

    if (!glfwVulkanSupported()) {
      throw std::runtime_error{"Glfw does not support vulkan"};
    }

    _window = std::make_unique<devices::window>(window_create_info{"Demo | 0 FPS", 1280, 720});
  }

  ~devices_module() override {
    _window.reset();

    glfwTerminate();
  }

  auto update() -> void override {
    glfwPollEvents();

    _elapsed_time += core::engine::delta_time();

    if (_elapsed_time >= units::second{1}) {
      _elapsed_time -= units::second{1};

      glfwSetWindowTitle(*_window, fmt::format("{} | {} FPS", _window->title(), _frame_count).c_str());

      _frame_count = 0;
    } else {
      ++_frame_count;
    }
  }

  auto window() -> devices::window& {
    return *_window;
  }

  auto required_instance_extensions() const -> std::vector<const char*> {
    auto extension_count = std::uint32_t{0};
    auto extensions = glfwGetRequiredInstanceExtensions(&extension_count);

    return std::vector<const char*>{extensions, extensions + extension_count};
  }

private:

  std::unique_ptr<devices::window> _window{};

  std::uint32_t _frame_count{};
  units::second _elapsed_time{};

}; // class devices_module

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_DEVICES_MODULE_HPP_
