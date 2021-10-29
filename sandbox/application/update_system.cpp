#include "update_system.hpp"

#include <core/events.hpp>

namespace sbx {

update_system::update_system(event_queue* event_queue, GLFWwindow* handle)
: _event_queue{event_queue},
  _handle{handle},
  _frame_counter{0u},
  _timer{0.0f} { }

void update_system::initialize() {
  _event_queue->add_listener<window_closed_event>([this](const auto&){
    finish();
  });
}

void update_system::update([[maybe_unused]] const time delta_time) {
  _timer += delta_time;

  if (_timer >= time{1}) {
    _event_queue->emplace_back<fps_updated_event>(_frame_counter);

    _timer = 0.0f;
    _frame_counter = 0u;
  }

  ++_frame_counter;

  glfwSwapBuffers(_handle);
  glfwPollEvents();
}

void update_system::finished() {

}

void update_system::aborted() {

}

} // namespace sbx
