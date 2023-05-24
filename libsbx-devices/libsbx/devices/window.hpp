#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <functional>
#include <unordered_set>

#include <libsbx/core/concepts.hpp>
#include <libsbx/core/delegate.hpp>

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

  /**
   * @brief Sets the callback for the @ref sbx::devices::window_closed_event event type
   * @tparam Callable 
   * @param callable 
   * @return 
   */
  template<core::callable<void, const window_closed_event&> Callable>
  auto set_on_window_closed(Callable&& callable) -> void {
    _on_window_closed = std::forward<Callable>(callable);
  }

  template<core::callable<void, const window_moved_event&> Callable>
  auto set_on_window_moved(Callable&& callable) -> void {
    _on_window_moved = std::forward<Callable>(callable);
  }

  template<core::callable<void, const window_resized_event&> Callable>
  auto set_on_window_resized(Callable&& callable) -> void {
    _on_window_resized = std::forward<Callable>(callable);
  }

  template<core::callable<void, const key_event&> Callable>
  auto set_on_key(Callable&& callable) -> void {
    _on_key = std::forward<Callable>(callable);
  }

  template<core::callable<void, const framebuffer_resized_event&> Callable>
  auto set_on_framebuffer_resized(Callable&& callable) -> void {
    _on_framebuffer_resized = std::forward<Callable>(callable);
  }

private:

  void _set_callbacks() {
    glfwSetWindowUserPointer(_handle, this);

    glfwSetWindowCloseCallback(_handle, [](GLFWwindow* window){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (self._on_window_closed) {
        const auto event = window_closed_event{};
        self._on_window_closed(event);
      }
    });

    glfwSetWindowPosCallback(_handle, [](GLFWwindow* window, std::int32_t x, std::int32_t y){
      const auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (self._on_window_moved) {
        const auto event = window_moved_event{x, y};
        self._on_window_moved(event);
      }
    });

    glfwSetWindowSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (self._on_window_resized) {
        const auto event = window_resized_event{width, height};
        self._on_window_resized(event);
      }
    });

    glfwSetFramebufferSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (self._on_framebuffer_resized) {
        const auto event = framebuffer_resized_event{width, height};
        self._on_framebuffer_resized(event);
        self._width = static_cast<std::uint32_t>(width);
        self._height = static_cast<std::uint32_t>(height);
      }
    });

    glfwSetKeyCallback(_handle, [](GLFWwindow* window, std::int32_t key, std::int32_t scancode, std::int32_t action, std::int32_t mods){
      auto& self = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (self._on_key) {
        const auto event = key_event{key, scancode, action, mods};
        self._on_key(event);
      }
    });
  }

  std::string _title{};
  std::uint32_t _width{};
  std::uint32_t _height{};

  GLFWwindow* _handle{};

  core::delegate<void(const window_closed_event&)> _on_window_closed{};
  core::delegate<void(const window_moved_event&)> _on_window_moved{};
  core::delegate<void(const window_resized_event&)> _on_window_resized{};
  core::delegate<void(const framebuffer_resized_event&)> _on_framebuffer_resized{};
  core::delegate<void(const key_event&)> _on_key{};

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
