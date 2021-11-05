#ifndef SBX_CORE_MODIFICATION_FLAG_HPP_
#define SBX_CORE_MODIFICATION_FLAG_HPP_

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

namespace sbx {

enum class modification_flag : uint8_t {
  shift     = GLFW_MOD_SHIFT,
  control   = GLFW_MOD_CONTROL,
  alt       = GLFW_MOD_ALT,
  super     = GLFW_MOD_SUPER,
  caps_lock = GLFW_MOD_CAPS_LOCK,
  num_lock  = GLFW_MOD_NUM_LOCK
}; // enum class modification_flag

using modifiers = uint8;

} // namespace sbx

#endif // SBX_CORE_MODIFICATION_FLAG_HPP_
