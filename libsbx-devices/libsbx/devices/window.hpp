#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <functional>
#include <unordered_set>
#include <cmath>

#include <libsbx/core/concepts.hpp>
#include <libsbx/core/delegate.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/devices/events.hpp>

namespace sbx::devices {

struct window_create_info {
  std::string title{};
  std::uint32_t width{};
  std::uint32_t height{};
}; // struct window_create_info

class window {

public:

  window(const window_create_info& create_info) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_VISIBLE, false);

    _title = create_info.title;
    _width = create_info.width;
    _height = create_info.height;

    _handle = glfwCreateWindow(static_cast<std::int32_t>(_width), static_cast<std::int32_t>(_height), _title.c_str(), nullptr, nullptr);

    if (!_handle) {
      throw std::runtime_error{"Could not create glfw window"};
    }

    glfwFocusWindow(_handle);

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

  auto window_closed_signal() -> signals::signal<window_closed_event>& {
    return _window_closed_signal;
  }

  auto window_moved_signal() -> signals::signal<window_moved_event>& {
    return _window_moved_signal;
  }

  auto window_resized_signal() -> signals::signal<window_resized_event>& {
    return _window_resized_signal;
  }

  auto framebuffer_resized_signal() -> signals::signal<framebuffer_resized_event>& {
    return _framebuffer_resized_signal;
  }

  auto key_pressed_signal() -> signals::signal<key_pressed_event>& {
    return _key_pressed_signal;
  }

  auto key_released_signal() -> signals::signal<key_released_event>& {
    return _key_released_signal;
  }

private:

  void _set_callbacks() {
    glfwSetWindowUserPointer(_handle, this);

    glfwSetWindowCloseCallback(_handle, [](GLFWwindow* window){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._window_closed_signal(window_closed_event{});
    });

    glfwSetWindowPosCallback(_handle, [](GLFWwindow* window, std::int32_t x, std::int32_t y){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._window_moved_signal(window_moved_event{x, y});
    });

    glfwSetWindowSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._window_resized_signal(window_resized_event{width, height});
    });

    glfwSetFramebufferSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      self._framebuffer_resized_signal(framebuffer_resized_event{width, height});

      self._width = static_cast<std::uint32_t>(width);
      self._height = static_cast<std::uint32_t>(height);
    });

    glfwSetKeyCallback(_handle, [](GLFWwindow* window, std::int32_t key, std::int32_t scancode, std::int32_t action, std::int32_t mods){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (action == GLFW_PRESS) {
        self._key_pressed_signal(key_pressed_event{key, scancode, action, mods});
      } else if (action == GLFW_RELEASE) {
        self._key_released_signal(key_released_event{key, scancode, action, mods});
      }
    });
  }

  std::string _title{};
  std::uint32_t _width{};
  std::uint32_t _height{};

  GLFWwindow* _handle{};

  signals::signal<window_closed_event> _window_closed_signal;
  signals::signal<window_moved_event> _window_moved_signal;
  signals::signal<window_resized_event> _window_resized_signal;
  signals::signal<framebuffer_resized_event> _framebuffer_resized_signal;
  signals::signal<key_pressed_event> _key_pressed_signal;
  signals::signal<key_released_event> _key_released_signal;

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
