#ifndef LIBSBX_DEVICES_DEVICES_MODULE_HPP_
#define LIBSBX_DEVICES_DEVICES_MODULE_HPP_

#include <cinttypes>

#include <memory>
#include <vector>
#include <unordered_map>

#include <fmt/format.h>

#include <GLFW/glfw3.h>

#include <libsbx/core/module.hpp>
#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/units/time.hpp>

#include <libsbx/devices/window.hpp>
#include <libsbx/devices/key.hpp>
#include <libsbx/devices/mouse_button.hpp>
#include <libsbx/devices/input.hpp>

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

    _window = std::make_unique<devices::window>(window_create_info{"Demo", 1280, 720});

    _window->on_key_pressed() += [](const key_pressed_event& event) {
      const auto key = static_cast<devices::key>(event.key);

      _current_key_states[key] = input_action::press;
    };

    _window->on_key_released() += [](const key_released_event& event) {
      const auto key = static_cast<devices::key>(event.key);

      _current_key_states[key] = input_action::release;
    };
  }

  ~devices_module() override {
    _window.reset();

    glfwTerminate();
  }

  auto update() -> void override {
    glfwPollEvents();

    std::swap(_current_key_states, _previous_key_states);
    _current_key_states.clear();
  }

  auto window() -> devices::window& {
    return *_window;
  }

  auto required_instance_extensions() const -> std::vector<const char*> {
    auto extension_count = std::uint32_t{0};
    auto extensions = glfwGetRequiredInstanceExtensions(&extension_count);

    return std::vector<const char*>{extensions, extensions + extension_count};
  }

  auto is_key_pressed(key key) const -> bool {
    if (auto current_entry = _current_key_states.find(key); current_entry != _current_key_states.end()) {
      if (auto previous_entry = _previous_key_states.find(key); previous_entry != _previous_key_states.end()) {
        return current_entry->second == input_action::press && previous_entry->second == input_action::release;
      }
    }

    return false;
  }

  auto is_key_released(key key) const -> bool {
    if (auto current_entry = _current_key_states.find(key); current_entry != _current_key_states.end()) {
      if (auto previous_entry = _previous_key_states.find(key); previous_entry != _previous_key_states.end()) {
        return current_entry->second == input_action::release && previous_entry->second == input_action::press;
      }
    }

    return false;
  }

private:

  inline static std::unordered_map<key, input_action> _current_key_states{};
  inline static std::unordered_map<key, input_action> _previous_key_states{};

  std::unique_ptr<devices::window> _window{};

}; // class devices_module

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_DEVICES_MODULE_HPP_
