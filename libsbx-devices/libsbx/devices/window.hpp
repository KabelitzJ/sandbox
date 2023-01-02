#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include <stdexcept>

namespace sbx::devices {

class window {

public:

  window() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, false);

    _handle = glfwCreateWindow(960, 720, "Demo", nullptr, nullptr);

    if (!_handle) {
      throw std::runtime_error{"Could not create glfw window"};
    }
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

private:

  GLFWwindow* _handle{};

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
