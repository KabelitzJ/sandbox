#include "window_module.hpp"

#include <sstream>

#include <core/events.hpp>
#include <core/key.hpp>
#include <core/logger.hpp>

#include "update_system.hpp"
#include "input_system.hpp"

namespace sbx {

window_module::window_module()
: _handle(nullptr) { }

void window_module::initialize()  {
  if (!glfwInit()) {
    logger::error("Could not initialite GLFW3");
    return;
  }

  glfwSetErrorCallback([](auto error, const auto* description) {
    logger::error("GLFW3 error({}): {}", error, description);
  });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  auto width = 1920;
  auto height = 1080;

  auto monitor = glfwGetPrimaryMonitor();

  if (monitor) {
    const auto mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
  }

  _handle = glfwCreateWindow(width, height, "Sandbox [FPS: 0]", monitor, nullptr);

  logger::debug("Window created with width {} height {}", width, height);

  if (!_handle) {
    logger::error("Could not create window");
    return;
  }

  dispatch_event<window_resized_event>(width, height);

  glfwSetWindowUserPointer(_handle, get_event_queue());

  glfwMakeContextCurrent(_handle);

  glfwSwapInterval(1);

  glfwRequestWindowAttention(_handle);

  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(_handle, GLFW_RAW_MOUSE_MOTION, true);
  }

  add_listener<toggle_mouse_visibility_event>([this](const auto&){
    if (glfwGetInputMode(_handle, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
      glfwSetInputMode(_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  });

  add_listener<fps_updated_event>([this](const auto& event){
    auto title = std::stringstream{};

    title << "Sandbox [FPS: " << event.fps << "]";

    glfwSetWindowTitle(_handle, title.str().c_str());
  });

  add_system<input_system>(_handle);
  add_system<update_system>(_handle);
}

void window_module::terminate() {
  glfwDestroyWindow(_handle);
  glfwTerminate();
}

} // namespace sbx
