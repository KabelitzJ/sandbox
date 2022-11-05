#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <string>
#include <stdexcept>

#include <GLFW/glfw3.h>

namespace sbx::devices {

struct window_create_info {
  std::string title{};
};

class window {

public:

  window() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(960, 720, "Sandbox", nullptr, nullptr);

    if (!_window) {
      throw std::runtime_error("Failed to create window");
    }
  }

  ~window() {
    glfwDestroyWindow(_window);
  }

  bool should_close() const {
    return glfwWindowShouldClose(_window);
  }

private:

  GLFWwindow* _window{};

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
