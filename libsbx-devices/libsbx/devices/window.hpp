#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <functional>
#include <filesystem>
#include <unordered_set>
#include <cmath>

#include <libsbx/core/concepts.hpp>
#include <libsbx/core/delegate.hpp>

#include <libsbx/math/vector2.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/devices/events.hpp>
#include <libsbx/devices/input.hpp>

namespace sbx::devices {

struct window_create_info {
  std::string title{};
  std::uint32_t width{};
  std::uint32_t height{};
}; // struct window_create_info

class window {

public:

  window(const window_create_info& create_info)
  : _last_mouse_position{-1.0f, -1.0f} {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // [NOTE] KAJ 2023-08-15 : Currently there seems to be a bug in the Vukan SDK version 1.3.250.1 that causes the validation layers to crash when resizing the window.
    // glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_VISIBLE, false);
    glfwWindowHint(GLFW_DECORATED, false);

    _title = create_info.title;
    // _width = create_info.width;
    // _height = create_info.height;

    auto* monitor = glfwGetPrimaryMonitor();

    if (!monitor) {
      throw std::runtime_error{"Could not get primary monitor"};
    }

    const auto* video_mode = glfwGetVideoMode(monitor);

    _width = video_mode->width;
    _height = video_mode->height;

    _handle = glfwCreateWindow(static_cast<std::int32_t>(_width), static_cast<std::int32_t>(_height), _title.c_str(), nullptr, nullptr);

    if (!_handle) {
      throw std::runtime_error{"Could not create glfw window"};
    }

    glfwSetWindowUserPointer(_handle, this);

    glfwFocusWindow(_handle);

    if (glfwRawMouseMotionSupported()) {
      glfwSetInputMode(_handle, GLFW_RAW_MOUSE_MOTION, true);
    }

    // glfwSetInputMode(_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetInputMode(_handle, GLFW_STICKY_KEYS, true);

    glfwSetInputMode(_handle, GLFW_LOCK_KEY_MODS, true);

    _set_callbacks();
  }

  ~window() {
    glfwDestroyWindow(_handle);
  }

  auto handle() -> GLFWwindow* {
    return _handle;
  }

  operator GLFWwindow*() {
    return _handle;
  }

  auto title() const -> const std::string& {
    return _title;
  }

  auto set_title(const std::string& title) -> void {
    _title = title;
    glfwSetWindowTitle(_handle, _title.c_str());
  }

  auto width() const -> std::uint32_t {
    return _width;
  }

  auto height() const -> std::uint32_t {
    return _height;
  }

  auto aspect_ratio() const -> std::float_t {
    return static_cast<std::float_t>(_width) / static_cast<std::float_t>(_height);
  }

  auto set_icon(const std::filesystem::path& path) -> void;

  /**
   * @brief Determins if the window should be closed
   * @return true if the window should be closed, false otherwise
   */
  auto should_close() -> bool {
    return glfwWindowShouldClose(_handle);
  }

  /**
   * @brief Makes the window visible
   */
  auto show() -> void {
    glfwShowWindow(_handle);
  }

  /**
   * @brief Hides the window
   */
  auto hide() -> void {
    glfwHideWindow(_handle);
  }

  auto is_iconified() const noexcept -> bool {
    return glfwGetWindowAttrib(_handle, GLFW_ICONIFIED);
  }

  auto is_focused() const noexcept -> bool {
    return glfwGetWindowAttrib(_handle, GLFW_FOCUSED);
  }

  auto is_visible() const noexcept -> bool {
    return glfwGetWindowAttrib(_handle, GLFW_VISIBLE);
  }

  auto on_window_closed_signal() -> signals::signal<const window_closed_event&>& {
    return _on_window_closed_signal;
  }

  auto on_window_moved_signal() -> signals::signal<const window_moved_event&>& {
    return _on_window_moved_signal;
  }

  auto on_window_resized_signal() -> signals::signal<const window_resized_event&>& {
    return _on_window_resized_signal;
  }

  auto on_framebuffer_resized() -> signals::signal<const framebuffer_resized_event&>& {
    return _on_framebuffer_resized;
  }

  auto on_key_pressed() -> signals::signal<const key_pressed_event&>& {
    return _on_key_pressed;
  }

  auto on_key_released() -> signals::signal<const key_released_event&>& {
    return _on_key_released;
  }

  auto on_mouse_moved() -> signals::signal<const mouse_moved_event&>& {
    return _on_mouse_moved;
  }

private:

  void _set_callbacks() {
    glfwSetWindowUserPointer(_handle, this);

    glfwSetWindowCloseCallback(_handle, [](GLFWwindow* window){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_window_closed_signal(window_closed_event{});
    });

    glfwSetWindowPosCallback(_handle, [](GLFWwindow* window, std::int32_t x, std::int32_t y){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_window_moved_signal(window_moved_event{x, y});
    });

    glfwSetWindowSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_window_resized_signal(window_resized_event{width, height});
    });

    glfwSetFramebufferSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_framebuffer_resized(framebuffer_resized_event{width, height});

      self._width = static_cast<std::uint32_t>(width);
      self._height = static_cast<std::uint32_t>(height);
    });

    glfwSetKeyCallback(_handle, [](GLFWwindow* window, std::int32_t key, [[maybe_unused]] std::int32_t scancode, std::int32_t action, std::int32_t mods){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (action == GLFW_PRESS) {
        input::_update_key_state(static_cast<devices::key>(key), input_action::press);
        self._on_key_pressed(key_pressed_event{static_cast<devices::key>(key), static_cast<devices::input_mod>(mods)});
      } else if (action == GLFW_RELEASE) {
        input::_update_key_state(static_cast<devices::key>(key), input_action::release);
        self._on_key_released(key_released_event{static_cast<devices::key>(key), static_cast<devices::input_mod>(mods)});
      }
    });

    glfwSetCursorPosCallback(_handle, [](auto* window, auto x, auto y){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      auto mouse_position = math::vector2{static_cast<std::float_t>(x), static_cast<std::float_t>(y)};

      if (self._last_mouse_position.x < 0.0f || self._last_mouse_position.y < 0.0f) {
        self._on_mouse_moved(mouse_moved_event{mouse_position.x, mouse_position.y});
      } else {
        self._on_mouse_moved(mouse_moved_event{mouse_position.x - self._last_mouse_position.x, mouse_position.y - self._last_mouse_position.y});
        self._last_mouse_position = mouse_position;
      }

      input::_update_mouse_position(mouse_position);
    });

    glfwSetMouseButtonCallback(_handle, [](auto* window, auto button, auto action, auto mods){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (action == GLFW_PRESS) {
        input::_update_mouse_button_state(static_cast<devices::mouse_button>(button), input_action::press);
        self._on_mouse_button_pressed(mouse_button_pressed_event{static_cast<devices::mouse_button>(button), static_cast<devices::input_mod>(mods)});
      } else if (action == GLFW_RELEASE) {
        input::_update_mouse_button_state(static_cast<devices::mouse_button>(button), input_action::release);
        self._on_mouse_button_released(mouse_button_released_event{static_cast<devices::mouse_button>(button), static_cast<devices::input_mod>(mods)});
      }
    });

    glfwSetScrollCallback(_handle, [](auto* window, auto x, auto y){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._on_mouse_scrolled(mouse_scrolled_event{static_cast<std::float_t>(x), static_cast<std::float_t>(y)});

      input::_update_scroll_delta(math::vector2{static_cast<std::float_t>(x), static_cast<std::float_t>(y)});
    }); 
  }

  std::string _title{};
  std::uint32_t _width{};
  std::uint32_t _height{};

  GLFWwindow* _handle{};

  math::vector2 _last_mouse_position;

  signals::signal<const window_closed_event&> _on_window_closed_signal;
  signals::signal<const window_moved_event&> _on_window_moved_signal;
  signals::signal<const window_resized_event&> _on_window_resized_signal;
  signals::signal<const framebuffer_resized_event&> _on_framebuffer_resized;
  signals::signal<const key_pressed_event&> _on_key_pressed;
  signals::signal<const key_released_event&> _on_key_released;
  signals::signal<const mouse_moved_event&> _on_mouse_moved;
  signals::signal<const mouse_button_pressed_event&> _on_mouse_button_pressed;
  signals::signal<const mouse_button_released_event&> _on_mouse_button_released;
  signals::signal<const mouse_scrolled_event&> _on_mouse_scrolled;

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
