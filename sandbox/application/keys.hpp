#ifndef SBX_APPLICATION_KEYS_HPP_
#define SBX_APPLICATION_KEYS_HPP_

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

namespace sbx {

enum class key_state : uint8 {
  pressed  = GLFW_PRESS,
  released = GLFW_RELEASE,
  repeated = GLFW_REPEAT
}; // enum class key_state

} // namespace sbx

#endif // SBX_APPLICATION_KEYS_HPP_
