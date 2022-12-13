#ifndef LIBSBX_DEVICES_MODIFIERS_HPP_
#define LIBSBX_DEVICES_MODIFIERS_HPP_

#include <cinttypes>

namespace sbx::devices {

class modifiers {

  friend class window;

public:

  static const modifiers shift;
  static const modifiers control;
  static const modifiers alt;
  static const modifiers super;
  static const modifiers caps_lock;
  static const modifiers num_lock;

  ~modifiers() = default;

  bool operator&(const modifiers& other) const noexcept;

  bool operator==(const modifiers& other) const noexcept;

private:

  modifiers(std::int32_t value);

  std::int32_t _value{};

}; // class modifiers

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_MODIFIERS_HPP_
