#ifndef DEMO_MODIFIERS_HPP_
#define DEMO_MODIFIERS_HPP_

#include <types/primitives.hpp>

namespace demo {

class modifiers {

  friend class window;
  friend class event_manager;

public:

  modifiers()
  : _value{} { }

  ~modifiers() = default;

  [[nodiscard]] bool is_shift_pressed() const noexcept {
    return _value & GLFW_MOD_SHIFT;
  }

  [[nodiscard]] bool is_ctrl_pressed() const noexcept {
    return _value & GLFW_MOD_CONTROL;
  }

  [[nodiscard]] bool is_alt_pressed() const noexcept {
    return _value & GLFW_MOD_ALT;
  }

  [[nodiscard]] bool is_super_pressed() const noexcept {
    return _value & GLFW_MOD_SUPER;
  }

  [[nodiscard]] bool is_caps_lock_pressed() const noexcept {
    return _value & GLFW_MOD_CAPS_LOCK;
  }

  [[nodiscard]] bool is_num_lock_pressed() const noexcept {
    return _value & GLFW_MOD_NUM_LOCK;
  }

private:

  modifiers(const sbx::int32 value)
  : _value{value} { }

  sbx::int32 _value{};

}; // class modifiers

} // namespace demo

#endif // DEMO_MODIFIERS_HPP_
