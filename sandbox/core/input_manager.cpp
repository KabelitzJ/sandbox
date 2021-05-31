#include "input_manager.hpp"

namespace sbx {

input_manager::input_manager(event_queue& event_queue) 
: _event_queue(event_queue),
 _key_states(),
 _button_states(),
 _mouse_position(0.0f, 0.0f) {
  _bind_callbacks();
}

input_manager::~input_manager() {

}

bool input_manager::is_key_pressed(key_code key) const {
  auto itr = _key_states.find(key);

  if (itr == _key_states.end()) {
    return false;
  }

  return itr->second;
}

bool input_manager::is_mouse_button_pressed(mouse_button button) const {
  auto itr = _button_states.find(button);

  if (itr == _button_states.end()) {
    return false;
  }

  return itr->second;
}

const glm::vec2& input_manager::mouse_position() const {
  return _mouse_position;
}

void input_manager::_bind_callbacks() {
  _event_queue.subscribe<key_pressed_event>([this](key_pressed_event& event){ _on_key_pressed_event(event); });
  _event_queue.subscribe<key_repeated_event>([this](key_repeated_event& event){ _on_key_repeated_event(event); });
  _event_queue.subscribe<key_released_event>([this](key_released_event& event){ _on_key_released_event(event); });
  _event_queue.subscribe<mouse_moved_event>([this](mouse_moved_event& event){ _on_mouse_moved_event(event); });
}

void input_manager::_on_key_pressed_event(key_pressed_event& event) {
  _key_states[event.code] = true;
}

void input_manager::_on_key_repeated_event(key_repeated_event& event) {
  _key_states[event.code] = true;
}

void input_manager::_on_key_released_event(key_released_event& event) {
  _key_states[event.code] = false;
}

void input_manager::_on_mouse_moved_event(mouse_moved_event& event) {
  _mouse_position = { event.x, event.y };
}

void input_manager::_on_mouse_button_pressed_event(mouse_button_pressed_event& event) {
  _button_states[event.button] = true;
}

void input_manager::_on_mouse_button_released_event(mouse_button_released_event& event) {
  _button_states[event.button] = false;
}


} // namespace sbx
