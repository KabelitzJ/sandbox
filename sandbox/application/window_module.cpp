#include "window_module.hpp"

#include "events.hpp"
#include "update_system.hpp"
#include "input_system.hpp"

namespace sbx {

window_module::window_module()
: _handle(nullptr) { }

window_module::~window_module() { }

void window_module::initialize()  {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _handle = glfwCreateWindow(960, 720, "Window", nullptr, nullptr);

  glfwSetWindowUserPointer(_handle, _event_queue);

  glfwMakeContextCurrent(_handle);

  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

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

  _scheduler->attach<update_system>(_event_queue, _handle);
  _scheduler->attach<input_system>(_event_queue, _handle);
}

void window_module::terminate() {
  glfwDestroyWindow(_handle);
  glfwTerminate();
}

} // namespace sbx
