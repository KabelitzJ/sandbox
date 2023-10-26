#ifndef LIBSBX_DEVICES_INPUT_HPP_
#define LIBSBX_DEVICES_INPUT_HPP_

#include <cinttypes>
#include <unordered_map>

#include <GLFW/glfw3.h>

#include <libsbx/utility/bitmask.hpp>

#include <libsbx/devices/key.hpp>
#include <libsbx/devices/mouse_button.hpp>
#include <libsbx/devices/input_action.hpp>
#include <libsbx/devices/input_mod.hpp>

namespace sbx::devices {

struct key_state {
  input_action action;
  input_action last_action;
}; // struct key_state

class input {

  friend class devices_module;
  friend class window;

public:

  input() = delete;

  static auto is_key_pressed(key key) -> bool;

  static auto is_key_down(key key) -> bool;

  static auto is_key_released(key key) -> bool;

private:

  static auto _transition_pressed_keys() -> void;

  static auto _update_key_state(key key, input_action action) -> void;

  static std::unordered_map<key, key_state> _key_states;

}; // class input

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_INPUT_HPP_
