#ifndef DEMO_MONITOR_HPP_
#define DEMO_MONITOR_HPP_

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>
#include <utils/noncopyable.hpp>
#include <utils/nonmovable.hpp>

#include "logger.hpp"
#include "event_manager.hpp"
#include "events.hpp"

namespace demo {

class monitor : public sbx::noncopyable, public sbx::nonmovable {

public:

  monitor(event_manager* event_manager)
  : _event_manager{event_manager},
    _handle{nullptr},
    _width{},
    _height{},
    _refresh_rate{} {
    _initialize();
  }

  ~monitor() = default;

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
    _handle = glfwGetPrimaryMonitor();

    if (!_handle) {
      throw std::runtime_error{"Failed to get primary monitor"};
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

  event_manager* _event_manager{};

  GLFWmonitor* _handle{};

  sbx::int32 _width{};
  sbx::int32 _height{};
  sbx::int32 _refresh_rate{};

}; // class monitor

} // namespace demo

#endif // DEMO_MONITOR_HPP_
