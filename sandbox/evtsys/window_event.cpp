#include "window_event.hpp"

namespace sbx {

event_type window_event::type() const {
  return event_type::window;
}

window_position_event::window_position_event(int x, int y)
: _x(x),
  _y(y) {

}

int window_position_event::x() const {
  return _x;
}

int window_position_event::y() const {
  return _y;
}

} // namespace sbx
