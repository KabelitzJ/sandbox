#ifndef LIBSBX_DEVICES_MONITOR_HPP_
#define LIBSBX_DEVICES_MONITOR_HPP_

#include <stdexcept>

#include <GLFW/glfw3.h>

namespace sbx::devices {

class monitor {

public:

  monitor() {
    _monitor = glfwGetPrimaryMonitor();

    if (!_monitor) {
      throw std::runtime_error{"Failed to get primary monitor"};
    }
  }

  ~monitor() = default;

private:

  GLFWmonitor* _monitor{};

}; // class monitor

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_MONITOR_HPP_
