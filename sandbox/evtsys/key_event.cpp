#include "key_event.hpp"

namespace sbx {

// key_pressed_event

key_pressed_event::key_pressed_event(key_code code)
: _code(code) {

}

event_type key_pressed_event::type() const {
  return event_type::KEY_PRESSED;
}

key_code key_pressed_event::code() const {
  return _code;
}

// key_pressed_event

key_repeated_event::key_repeated_event(key_code code)
: _code(code) {

}

event_type key_repeated_event::type() const {
  return event_type::KEY_PRESSED;
}

key_code key_repeated_event::code() const {
  return _code;
}

// key_pressed_event

key_released_event::key_released_event(key_code code)
: _code(code) {

}

event_type key_released_event::type() const {
  return event_type::KEY_PRESSED;
}

key_code key_released_event::code() const {
  return _code;
}

} // namespace sbx
