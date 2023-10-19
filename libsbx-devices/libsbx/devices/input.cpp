#include <libsbx/devices/input.hpp>

#include <libsbx/devices/devices_module.hpp>

namespace sbx::devices {

std::unordered_map<key, key_state> input::_key_states;

auto input::is_key_pressed(key key) -> bool {
  if (auto entry = _key_states.find(key); entry != _key_states.end()) {
    return entry->second.action == input_action::press;
  }

  return false;
}

auto input::is_key_released(key key) -> bool {
  if (auto entry = _key_states.find(key); entry != _key_states.end()) {
    return entry->second.action == input_action::release;
  }

  return false;
}

auto input::_update_key_state(key key, input_action action) -> void {
  auto& state = _key_states[key];

  state.last_action = state.action;
  state.action = action;
}

} // namespace sbx::devices
