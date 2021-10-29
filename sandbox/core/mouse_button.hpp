#ifndef SBX_CORE_MOUSE_BUTTON_HPP_
#define SBX_CORE_MOUSE_BUTTON_HPP_

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

namespace sbx {

enum class mouse_button : uint8_t {
  one     = GLFW_MOUSE_BUTTON_1,
  two     = GLFW_MOUSE_BUTTON_2,
  three   = GLFW_MOUSE_BUTTON_3,
  four    = GLFW_MOUSE_BUTTON_4,
  five    = GLFW_MOUSE_BUTTON_5,
  six     = GLFW_MOUSE_BUTTON_6,
  seven   = GLFW_MOUSE_BUTTON_7,
  eight   = GLFW_MOUSE_BUTTON_8,
  left    = GLFW_MOUSE_BUTTON_LEFT,
  right   = GLFW_MOUSE_BUTTON_RIGHT,
  middle  = GLFW_MOUSE_BUTTON_MIDDLE,
}; // enum class mouse_buttons

} // namespace sbx

#endif // SBX_CORE_MOUSE_BUTTON_HPP_
