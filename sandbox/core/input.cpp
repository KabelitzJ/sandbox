#include "input.hpp"

#include "events.hpp"

namespace sbx {

input::input(event_queue* event_queue)
: _event_queue{event_queue},
  _is_first_mouse_movement{true},
  _mouse_offset{0.0f, 0.0f},
  _mouse_position{0.0f, 0.0f},
  _key_states{},
  _mouse_button_states{} {
  _initialize();
}

input::~input() {

}

vector2 input::mouse_offset() const {
  return _mouse_offset;
}

vector2 input::mouse_position() const {
  return _mouse_position;
}

bool input::is_key_down(const key key) const {
  if (const auto state = _key_states.find(key); state != _key_states.cend()) {
    return state->second;
  }

  return false;
}

bool input::is_key_up(const key key) const {
  if (const auto state = _key_states.find(key); state != _key_states.cend()) {
    return !state->second;
  }

  return true;
}

bool input::is_mouse_button_down(const mouse_button button) const {
  if (const auto state = _mouse_button_states.find(button); state != _mouse_button_states.cend()) {
    return state->second;
  }

  return false;
}

bool input::is_mouse_button_up(const mouse_button button) const {
  if (const auto state = _mouse_button_states.find(button); state != _mouse_button_states.cend()) {
    return !state->second;
  }

  return true;
}

void input::_initialize() {
  _event_queue->add_listener<mouse_moved_event>([this](const auto& event) {
    if (_is_first_mouse_movement) {
      _is_first_mouse_movement = false;
      _mouse_position = event.position;
    }

    _mouse_offset = event.position - _mouse_position;
    _mouse_position = event.position;
  });

  _event_queue->add_listener<key_pressed_event>([this](const auto& event) {
    _key_states[event.key_code] = true;
  });

  _event_queue->add_listener<key_repeated_event>([this](const auto& event) {
    _key_states[event.key_code] = true;
  });

  _event_queue->add_listener<key_released_event>([this](const auto& event) {
    _key_states[event.key_code] = false;
  });

  _event_queue->add_listener<mouse_button_pressed_event>([this](const auto& event) {
    _mouse_button_states[event.button_code] = true;
  });

  _event_queue->add_listener<mouse_button_repeated_event>([this](const auto& event) {
    _mouse_button_states[event.button_code] = true;
  });

  _event_queue->add_listener<mouse_button_released_event>([this](const auto& event) {
    _mouse_button_states[event.button_code] = false;
  });
}

} // namespace sbx