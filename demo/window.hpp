#ifndef DEMO_WINDOW_HPP_
#define DEMO_WINDOW_HPP_

#include <string>

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

namespace demo {

class window {

public:

  window(const std::string& title, const sbx::int32 width, const sbx::int32 height)
  : _title{title},
    _width{width},
    _height{height},
    _handle{nullptr} {
    _initialize_glfw();
  }

  window(const window& other) = delete;

  ~window() {
    _terminate_glfw();
  }

  window& operator=(const window& other) = delete;

  bool should_close() const {
    return glfwWindowShouldClose(_handle);
  }

  void poll_events() {
    glfwPollEvents();
  }

private:

  void _initialize_glfw() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _handle = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
  }

  void _terminate_glfw() {
    glfwDestroyWindow(_handle);
    glfwTerminate();
  }

  std::string _title{};
  sbx::int32 _width{};
  sbx::int32 _height{};
  GLFWwindow* _handle{};

}; // class window

} // namespace demo

#endif // DEMO_WINDOW_HPP_
