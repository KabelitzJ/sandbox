#include <libsbx/devices/input.hpp>

#include <libsbx/devices/devices_module.hpp>

namespace sbx::devices {

std::unordered_map<key, key_state> input::_key_states;

auto input::is_key_pressed(key key) -> bool {
  if (auto entry = _key_states.find(key); entry != _key_states.end()) {
    const auto& state = entry->second;

    return state.action == input_action::press;
  }

  return false;
}

auto input::is_key_down(key key) -> bool {
  if (auto entry = _key_states.find(key); entry != _key_states.end()) {
    const auto& state = entry->second;

    return state.action == input_action::press || state.action == input_action::repeat;
  }

  return false;
}

auto input::is_key_released(key key) -> bool {
  if (auto entry = _key_states.find(key); entry != _key_states.end()) {
    const auto& state = entry->second;

    return state.action == input_action::release;
  }

  return false;
}

auto input::_transition_pressed_keys() -> void {
  for (auto& [key, key_state] : _key_states) {
    if (key_state.action == input_action::press) {
      key_state.action = input_action::repeat;
    }
  }
}

auto input::_transition_pressed_mouse_buttons() -> void {
  for (auto& [button, key_state] : _mouse_button_states) {
    if (key_state.action == input_action::press) {
      key_state.action = input_action::repeat;
    }
  }
}

auto input::_update_key_state(key key, input_action action) -> void {
  auto entry = _key_states.find(key);

  if (entry == _key_states.end()) {
    entry = _key_states.insert({key, key_state{input_action::release, input_action::release}}).first;
  }

  auto& state = entry->second;

  state.last_action = state.action;
  state.action = action;
}

auto input::_update_mouse_button_state(mouse_button button, input_action action) -> void {
  auto entry = _mouse_button_states.find(button);

  if (entry == _mouse_button_states.end()) {
    entry = _mouse_button_states.insert({button, key_state{input_action::release, input_action::release}}).first;
  }

  auto& state = entry->second;

  state.last_action = state.action;
  state.action = action;
}

auto input::_update_mouse_position(const math::vector2u& position) -> void {

}

} // namespace sbx::devices
