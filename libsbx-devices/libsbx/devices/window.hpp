#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <functional>

#include <libsbx/devices/events.hpp>

namespace sbx::devices {

/**
 * @brief  Describes a type or object that can be invoked with the give parameters and return the given type
 * 
 * @tparam Callable Type of the callable
 * @tparam Return Return type of the callable
 * @tparam Args... Types of the arguments of the callable
 */
template<typename Callable, typename Return, typename... Args>
concept callable = std::is_invocable_r_v<Return, Callable, Args...>;

class window {

public:

  window(const std::string& title, std::uint32_t width, std::uint32_t height) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_VISIBLE, false);

    _title = title;
    _width = width;
    _height = height;

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
  template<callable<void, const window_closed_event&> Callable>
  auto set_on_window_closed(Callable&& callable) -> void {
    _on_window_closed = std::forward<Callable>(callable);
  }

  template<callable<void, const window_moved_event&> Callable>
  auto set_on_window_moved(Callable&& callable) -> void {
    _on_window_moved = std::forward<Callable>(callable);
  }

  template<callable<void, const window_resized_event&> Callable>
  auto set_on_window_resized(Callable&& callable) -> void {
    _on_window_resized = std::forward<Callable>(callable);
  }

private:

  void _set_callbacks() {
    glfwSetWindowUserPointer(_handle, this);

    glfwSetWindowCloseCallback(_handle, [](GLFWwindow* window){
      const auto& user_data = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (user_data._on_window_closed) {
        const auto event = window_closed_event{};
        user_data._on_window_closed(event);
      }
    });

    glfwSetWindowPosCallback(_handle, [](GLFWwindow* window, std::int32_t x, std::int32_t y){
      const auto& user_data = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (user_data._on_window_moved) {
        const auto event = window_moved_event{x, y};
        user_data._on_window_moved(event);
      }
    });

    glfwSetWindowSizeCallback(_handle, [](GLFWwindow* window, std::int32_t width, std::int32_t height){
      const auto& user_data = *static_cast<devices::window*>(glfwGetWindowUserPointer(window));

      if (user_data._on_window_resized) {
        const auto event = window_resized_event{width, height};
        user_data._on_window_resized(event);
      }
    });
  }

  std::string _title{};
  std::uint32_t _width{};
  std::uint32_t _height{};

  GLFWwindow* _handle{};

  std::function<void(const window_closed_event&)> _on_window_closed{};
  std::function<void(const window_moved_event&)> _on_window_moved{};
  std::function<void(const window_resized_event&)> _on_window_resized{};

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
