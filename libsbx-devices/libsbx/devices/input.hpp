#ifndef LIBSBX_DEVICES_INPUT_HPP_
#define LIBSBX_DEVICES_INPUT_HPP_

#include <cinttypes>

#include <GLFW/glfw3.h>

#include <libsbx/utility/bitmask.hpp>

#include <libsbx/devices/key.hpp>
#include <libsbx/devices/mouse_button.hpp>

namespace sbx::devices {

enum class input_action : std::int32_t {
  release = GLFW_RELEASE,
  press = GLFW_PRESS,
  repeat = GLFW_REPEAT,
}; // enum class input_action

enum class input_mod : std::int32_t {
  shift = GLFW_MOD_SHIFT,
  control = GLFW_MOD_CONTROL,
  alt = GLFW_MOD_ALT,
  super = GLFW_MOD_SUPER,
  caps_lock = GLFW_MOD_CAPS_LOCK,
  num_lock = GLFW_MOD_NUM_LOCK,
}; // enum class input_mod

class input {

public:

  static auto is_key_pressed(key key) -> bool;

  static auto is_key_released(key key) -> bool;

private:

}; // class input

} // namespace sbx::devices

template<>
struct sbx::utility::enable_bitmask_operators<sbx::devices::input_action> : std::true_type { };

template<>
struct sbx::utility::enable_bitmask_operators<sbx::devices::input_mod> : std::true_type { };

#endif // LIBSBX_DEVICES_INPUT_HPP_
