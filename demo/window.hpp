#ifndef DEMO_WINDOW_HPP_
#define DEMO_WINDOW_HPP_

#include <string>

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

#include "logger.hpp"
#include "configuration.hpp"

namespace demo {

class window {

public:

  window(logger* logger, configuration* configuration)
  : _logger{logger},
    _configuration{configuration},
    _title{_configuration->get<std::string>("name")},
    _width{_configuration->get<sbx::int32>("window.resolution.width")},
    _height{_configuration->get<sbx::int32>("window.resolution.height")},
    _handle{nullptr} {
    _initialize_glfw();
  }

  window(const window& other) = delete;

  window(window&& other) noexcept = delete;

  ~window() {
    _terminate_glfw();
  }

  window& operator=(const window& other) = delete;

  window& operator=(window&& other) noexcept = delete;

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

  logger* _logger{};
  configuration* _configuration{};

  std::string _title{};
  sbx::int32 _width{};
  sbx::int32 _height{};
  GLFWwindow* _handle{};


}; // class window

} // namespace demo

#endif // DEMO_WINDOW_HPP_
