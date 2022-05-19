#ifndef DEMO_WINDOW_HPP_
#define DEMO_WINDOW_HPP_

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

#include "logger.hpp"
#include "configuration.hpp"
#include "event_manager.hpp"
#include "monitor.hpp"
#include "events.hpp"

namespace demo {

class window {

public:

  window(logger* logger, configuration* configuration, event_manager* event_manager, monitor* monitor)
  : _logger{logger},
    _configuration{configuration},
    _event_manager{event_manager},
    _monitor{monitor},
    _handle{nullptr},
    _is_fullscreen{false} {
    _initialize();
  }

  window(const window&) = delete;

  window(window&&) = delete;

  ~window() {
    _terminate();
  }

  window& operator=(const window&) = delete;

  window& operator=(window&&) = delete;

  void poll_events() {
    glfwPollEvents();
  }

  void swap_buffers() {
    glfwSwapBuffers(_handle);
  }

  void set_fullscreen() {
    if (_is_fullscreen) {
      return;
    }

    glfwSetWindowMonitor(_handle, _monitor->handle(), 0, 0, _monitor->width(), _monitor->height(), _monitor->refresh_rate());

    _is_fullscreen = true;
  }

  void set_windowed() {
    if (!_is_fullscreen) {
      return;
    }

    const auto monitor_width = _monitor->width();
    const auto monitor_height = _monitor->height();

    const auto window_x = static_cast<sbx::int32>(monitor_width * 0.05);
    const auto window_y = static_cast<sbx::int32>(monitor_height * 0.05);

    const auto window_width = static_cast<sbx::int32>(monitor_width * 0.9);
    const auto window_height = static_cast<sbx::int32>(monitor_height * 0.9);

    glfwSetWindowMonitor(_handle, nullptr, window_x, window_y, window_width, window_height, _monitor->refresh_rate());

    _is_fullscreen = false;
  }

  void set_title(const std::string& title) {
    glfwSetWindowTitle(_handle, title.c_str());
  }

  void set_size(const sbx::int32 width, const sbx::int32 height) {
    glfwSetWindowSize(_handle, width, height);
  }

  void set_position(const sbx::int32 x, const sbx::int32 y) {
    glfwSetWindowPos(_handle, x, y);
  }

private:

  void _initialize() {
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

    set_fullscreen();
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

    glfwSetWindowCloseCallback(_handle, [](auto* handle) {
      auto evt_manager = static_cast<event_manager*>(glfwGetWindowUserPointer(handle));
      evt_manager->dispatch<window_closed_event>();
    });

    glfwSetWindowIconifyCallback(_handle, [](auto* handle, auto iconified) {
      auto evt_manager = static_cast<event_manager*>(glfwGetWindowUserPointer(handle));

      if (iconified) {
        evt_manager->dispatch<window_minimized_event>();
      } else {
        evt_manager->dispatch<window_restored_event>();
      }
    });

    glfwSetWindowMaximizeCallback(_handle, [](auto* handle, auto maximized) {
      auto evt_manager = static_cast<event_manager*>(glfwGetWindowUserPointer(handle));

      if (maximized) {
        evt_manager->dispatch<window_maximized_event>();
      } else {
        evt_manager->dispatch<window_restored_event>();
      }
    });

    glfwSetKeyCallback(_handle, [](auto* handle, auto key, [[maybe_unused]] auto scancode, auto action, auto mods) {
      auto evt_manager = static_cast<event_manager*>(glfwGetWindowUserPointer(handle));

      if (action == GLFW_PRESS) {
        evt_manager->dispatch<key_pressed_event>(key, mods);
      } else if (action == GLFW_RELEASE) {
        evt_manager->dispatch<key_released_event>(key, mods);
      }
    });
  }

  void _terminate() {
    glfwDestroyWindow(_handle);
  }

  logger* _logger{};
  configuration* _configuration{};
  event_manager* _event_manager{};
  monitor* _monitor{};

  GLFWwindow* _handle{};

  bool _is_fullscreen{};

}; // class window

} // namespace demo

#endif // DEMO_WINDOW_HPP_
