#include "window.hpp"

#include <stdexcept>

#include <core/logger.hpp>

namespace sbx {

window::window()
: _context{},
  _handle{nullptr} {
  _initialize();
}

window::~window() {
  _terminate();
}

void window::swap_buffers() {
  glfwSwapBuffers(_handle);
}

void window::_initialize() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  auto monitor = glfwGetPrimaryMonitor();

  if (!monitor) {
    logger::critical("Failed to get primary monitor");
    throw std::runtime_error("Failed to get primary monitor");
  }

  const auto video_mode = glfwGetVideoMode(monitor);

  _handle = glfwCreateWindow(video_mode->width, video_mode->height, "Sandbox", monitor, nullptr);

  if (!_handle) {
    logger::critical("Failed to create window");
    throw std::runtime_error("Failed to create window");
  }

  glfwMakeContextCurrent(_handle);

  glfwSwapInterval(1);
}

void window::_terminate() {
  glfwDestroyWindow(_handle);
}

} // namespace sbx
