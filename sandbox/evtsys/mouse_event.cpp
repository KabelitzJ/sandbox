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

mouse_button_pressed_event::mouse_button_pressed_event(mouse_button button)
: _button(button) {

}

event_type mouse_button_pressed_event::type() const {
  return event_type::BUTTON_PRESSED;
}

mouse_button mouse_button_pressed_event::button() const {
  return _button;
}

mouse_button_released_event::mouse_button_released_event(mouse_button button)
: _button(button) {

}

event_type mouse_button_released_event::type() const {
  return event_type::BUTTON_RELEASED;
}

mouse_button mouse_button_released_event::button() const {
  return _button;
}

} // namespace sbx 
