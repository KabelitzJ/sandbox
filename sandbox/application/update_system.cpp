#include "update_system.hpp"

#include "events.hpp"

namespace sbx {

update_system::update_system(event_queue* event_queue, GLFWwindow* handle)
: _event_queue{event_queue},
  _handle{handle} { }

void update_system::initialize() {
  _event_queue->add_listener<window_closed_event>([this](const auto&){
    finish();
  });
}

void update_system::update([[maybe_unused]] const time delta_time) {
  glfwSwapBuffers(_handle);
  glfwPollEvents();
}

void update_system::finished() {

}

void update_system::aborted() {

}

} // namespace sbx
