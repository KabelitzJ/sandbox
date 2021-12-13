#include "input.hpp"

#include "events.hpp"

namespace sbx {

input::input(event_queue* event_queue)
: _event_queue{event_queue},
  _is_first_mouse_movement{true},
  _mouse_position{0, 0} {
  _initialize();
}

input::~input() {

}

vector2 input::mouse_position() const {
  return _mouse_position;
}

void input::_initialize() {
  _event_queue->add_listener<mouse_moved_event>([this](const auto& event) {
    _mouse_position = event.position;
  });
}

} // namespace sbx