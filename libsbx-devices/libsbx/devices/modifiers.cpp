#include <libsbx/devices/modifiers.hpp>

#include <GLFW/glfw3.h>

namespace sbx::devices {

const modifiers modifiers::shift{GLFW_MOD_SHIFT};
const modifiers modifiers::control{GLFW_MOD_CONTROL};
const modifiers modifiers::alt{GLFW_MOD_ALT};
const modifiers modifiers::super{GLFW_MOD_SUPER};
const modifiers modifiers::caps_lock{GLFW_MOD_CAPS_LOCK};
const modifiers modifiers::num_lock{GLFW_MOD_NUM_LOCK};

bool modifiers::operator&(const modifiers& other) const noexcept {
  return _value & other._value;
}

bool modifiers::operator==(const modifiers& other) const noexcept {
  return _value == other._value;
}

modifiers::modifiers(std::int32_t value)
: _value{value} { }

} // namespace sbx::devices
