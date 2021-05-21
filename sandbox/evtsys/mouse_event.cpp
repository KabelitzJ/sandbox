#include "mouse_event.hpp"

namespace sbx {

mouse_moved_event::mouse_moved_event(float x, float y)
: _x(x),
  _y(y) {

}

event_type mouse_moved_event::type() const {
  return event_type::MOUSE_MOVED;
}

float mouse_moved_event::x() const {
  return _x;
}

float mouse_moved_event::y() const {
  return _y;
}

} // namespace sbx 
