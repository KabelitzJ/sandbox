#ifndef DEMO_MONITOR_HPP_
#define DEMO_MONITOR_HPP_

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

#include "logger.hpp"
#include "event_manager.hpp"
#include "events.hpp"

namespace demo {

class monitor {

public:

  monitor(logger* logger, event_manager* event_manager)
  : _logger{logger},
    _event_manager{event_manager},
    _handle{nullptr},
    _width{},
    _height{},
    _refresh_rate{} {
    _initialize();
  }

  monitor(const monitor&) = delete;

  monitor(monitor&&) = delete;

  ~monitor() {
    _terminate();
  }

  monitor& operator=(const monitor&) = delete;

  monitor& operator=(monitor&&) = delete;

  [[nodiscard]] sbx::int32 width() const noexcept {
    return _width;
  }

  [[nodiscard]] sbx::int32 height() const noexcept {
    return _height;
  }

  [[nodiscard]] sbx::int32 refresh_rate() const noexcept {
    return _refresh_rate;
  }

  [[nodiscard]] GLFWmonitor* handle() const noexcept {
    return _handle;
  }

private:

  void _initialize() {
    if (!glfwInit()) {
      _logger->error("Failed to initialize GLFW");
      return;
    }

    _handle = glfwGetPrimaryMonitor();

    if (!_handle) {
      _logger->error("Failed to get primary monitor");
      return;
    }

    const auto* video_mode = glfwGetVideoMode(_handle);

    _width = video_mode->width;
    _height = video_mode->height;
    _refresh_rate = video_mode->refreshRate;

    glfwSetMonitorUserPointer(_handle, _event_manager);

    glfwSetMonitorCallback([](GLFWmonitor* handle, int event) {
      auto evt_manager = static_cast<event_manager*>(glfwGetMonitorUserPointer(handle));

      if (event == GLFW_CONNECTED) {
        evt_manager->dispatch<monitor_connected_event>();
      } else if (event == GLFW_DISCONNECTED) {
        evt_manager->dispatch<monitor_disconnected_event>();
      }
    });
  }

  void _terminate() {
    glfwTerminate();
  }

  logger* _logger{};
  event_manager* _event_manager{};

  GLFWmonitor* _handle{};

  sbx::int32 _width{};
  sbx::int32 _height{};
  sbx::int32 _refresh_rate{};

}; // class monitor

} // namespace demo

#endif // DEMO_MONITOR_HPP_
