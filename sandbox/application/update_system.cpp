#include "update_system.hpp"

#include "events.hpp"

namespace sbx {

update_system::update_system(event_queue* event_queue, GLFWwindow* handle)
: _event_queue{event_queue},
  _handle{handle} { }

void update_system::initialize() {

}

void update_system::update([[maybe_unused]] const time delta_time) {
  if (glfwWindowShouldClose(_handle)) {
    _event_queue->emplace_back<window_closed_event>();
    finish();
    return;
  }

  glfwSwapBuffers(_handle);
  glfwPollEvents();
}

void update_system::finished() {

}

void update_system::aborted() {

}

} // namespace sbx
