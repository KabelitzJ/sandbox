#include "window_module.hpp"

#include <sstream>

#include <core/events.hpp>
#include <core/key.hpp>

#include "update_system.hpp"
#include "input_system.hpp"

namespace sbx {

window_module::window_module()
: _handle(nullptr) { }

void window_module::initialize()  {
  _logger->info("Initializing window module...");

  if (!glfwInit()) {
    _logger->critical("Could not initialite GLFW3");
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _handle = glfwCreateWindow(960, 720, "Sandbox [FPS: 0]", nullptr, nullptr);

  if (!_handle) {
    _logger->critical("Could not create window");
    return;
  }

  glfwSetWindowUserPointer(_handle, _event_queue);

  glfwMakeContextCurrent(_handle);

  glfwSwapInterval(0);

  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(_handle, GLFW_RAW_MOUSE_MOTION, true);
  }

  _event_queue->add_listener<toggle_mouse_visibility_event>([this](const auto&){
    if (glfwGetInputMode(_handle, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
      glfwSetInputMode(_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  });

  _event_queue->add_listener<fps_updated_event>([this](const auto& event){
    auto title = std::stringstream{};

    title << "Sandbox [FPS: " << event.fps << "]";

    glfwSetWindowTitle(_handle, title.str().c_str());
  });

  _scheduler->add_system<update_system>(_event_queue, _handle);
  _scheduler->add_system<input_system>(_event_queue, _handle);
}

void window_module::terminate() {
  glfwDestroyWindow(_handle);
  glfwTerminate();

  _logger->info("Terminating window module...");
}

} // namespace sbx
