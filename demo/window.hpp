#ifndef DEMO_WINDOW_HPP_
#define DEMO_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

#include "logger.hpp"
#include "configuration.hpp"
#include "event_manager.hpp"
#include "events.hpp"

namespace demo {

class window {

public:

  window(logger* logger, configuration* configuration, event_manager* event_manager)
  : _logger{logger},
    _configuration{configuration},
    _event_manager{event_manager},
    _handle{nullptr} {
    _initialize();
  }

  window(const window& other) = delete;

  window(window&& other) = delete;

  ~window() {
    _terminate();
  }

  window& operator=(const window& other) = delete;

  window& operator=(window&& other) = delete;

  void poll_events() {
    glfwPollEvents();
  }

  bool should_close() const {
    return glfwWindowShouldClose(_handle);
  }

private:

  void _initialize() {
    if (!glfwInit()) {
      _logger->error("Failed to initialize GLFW");
      return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    const auto name = _configuration->get<std::string>("name");
    const auto width = _configuration->get<sbx::int32>("window.resolution.width");
    const auto height = _configuration->get<sbx::int32>("window.resolution.height");

    _handle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    if (!_handle) {
      _logger->error("Failed to create window");
      return;
    }

    glfwSetWindowUserPointer(_handle, _event_manager);

    _setup_callbacks();
  }

  void _setup_callbacks() {
    glfwSetWindowSizeCallback(_handle, [](auto* handle, auto w, auto h) {
      auto evt_manager = static_cast<event_manager*>(glfwGetWindowUserPointer(handle));
      evt_manager->dispatch<window_resized_event>(w, h);
    });

    glfwSetWindowPosCallback(_handle, [](auto* handle, auto x, auto y) {
      auto evt_manager = static_cast<event_manager*>(glfwGetWindowUserPointer(handle));
      evt_manager->dispatch<window_moved_event>(x, y);
    });

    glfwSetCursorPosCallback(_handle, [](auto* handle, auto x, auto y) {
      auto evt_manager = static_cast<event_manager*>(glfwGetWindowUserPointer(handle));
      evt_manager->dispatch<mouse_moved_event>(static_cast<sbx::int32>(x), static_cast<sbx::int32>(y));
    });
  }

  void _terminate() {
    glfwDestroyWindow(_handle);
    glfwTerminate();
  }

  logger* _logger{};
  configuration* _configuration{};
  event_manager* _event_manager{};

  GLFWwindow* _handle{};

}; // class window

} // namespace demo

#endif // DEMO_WINDOW_HPP_
