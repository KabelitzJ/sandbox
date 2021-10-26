#ifndef SBX_APPLICATION_KEYS_HPP_
#define SBX_APPLICATION_KEYS_HPP_

#include <GLFW/glfw3.h>

#include <types/primitives.hpp>

#include <utils/enum.hpp>

namespace sbx {

enum class key_state : uint8 {
  pressed  = GLFW_PRESS,
  released = GLFW_RELEASE,
  repeated = GLFW_REPEAT
}; // enum class key_state

template<>
struct is_implicit_enum_class<key_state> : std::true_type { };

} // namespace sbx

#endif // SBX_APPLICATION_KEYS_HPP_
