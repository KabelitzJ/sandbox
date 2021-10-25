#include "window_module.hpp"

#include "events.hpp"
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

  _event_queue->add_listener<key_event>([this](const auto& event){
    if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS) {
      glfwSetWindowShouldClose(_handle, true);
    }
  });

  _scheduler->attach([this](const auto, auto finish){
    if (glfwWindowShouldClose(_handle)) {
      _event_queue->emplace_back<window_closed_event>();
      finish();
      return;
    }

    glfwSwapBuffers(_handle);
    glfwPollEvents();
  });

  _scheduler->attach<input_system>(_event_queue, _handle);
}

void window_module::terminate() {
  glfwDestroyWindow(_handle);
  glfwTerminate();
}

} // namespace sbx
