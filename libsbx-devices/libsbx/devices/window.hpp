#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <functional>

#include <libsbx/devices/events.hpp>

namespace sbx::devices {

template<typename Callable, typename Return, typename... Args>
concept callable = std::is_invocable_r_v<Return, Callable, Args...>;

class window {

public:

  window() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, false);

    _handle = glfwCreateWindow(960, 720, "Demo", nullptr, nullptr);

    if (!_handle) {
      throw std::runtime_error{"Could not create glfw window"};
    }

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

  auto should_close() -> bool {
    return glfwWindowShouldClose(_handle);
  }

  template<callable<void, const window_closed_event&> Callable>
  void set_on_window_closed(Callable&& callable) {
    _on_window_closed = std::forward<Callable>(callable);
  }

  template<callable<void, const window_moved_event&> Callable>
  void set_on_window_closed(Callable&& callable) {
    _on_window_moved = std::forward<Callable>(callable);
  }

  template<callable<void, const window_resized_event&> Callable>
  void set_on_window_closed(Callable&& callable) {
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

  GLFWwindow* _handle{};
  std::function<void(const window_closed_event&)> _on_window_closed{};
  std::function<void(const window_moved_event&)> _on_window_moved{};
  std::function<void(const window_resized_event&)> _on_window_resized{};

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
