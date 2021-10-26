#include "update_system.hpp"

namespace sbx {

update_system::update_system(event_queue* event_queue, GLFWwindow* handle)
: _event_queue{event_queue},
  _handle{handle} { }

void input_system::initialize() {

}

void input_system::update([[maybe_unused]] const time delta_time) {
  if (glfwWindowShouldClose(_handle)) {
    _event_queue->emplace_back<window_closed_event>();
    finish();
    return;
  }

  glfwSwapBuffers(_handle);
  glfwPollEvents();
}

void input_system::finished() {

}

void input_system::aborted() {

}

} // namespace sbx
