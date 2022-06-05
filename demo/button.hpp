#ifndef DEMO_BUTTON_HPP_
#define DEMO_BUTTON_HPP_

#include <type_traits>
#include <functional>

#include <types/primitives.hpp>

namespace demo {

class button {

  friend class window;
  friend class event_manager;

  friend class std::hash<button>;

  friend constexpr bool operator==(const button& lhs, const button& rhs) noexcept;

public:

  static const button left;
  static const button right;
  static const button middle;

  ~button() = default;

private:

  button(const sbx::int32 value)
  : _value{value} { }

  sbx::int32 _value{};

}; // class button

[[nodiscard]] constexpr bool operator==(const button& lhs, const button& rhs) noexcept {
  return lhs._value == rhs._value;
}

} // namespace demo

template<>
struct std::hash<demo::button> {
  [[nodiscard]] std::size_t operator()(const demo::button& key) const noexcept {
    return std::hash<sbx::int32>{}(key._value);
  }
};

#endif // DEMO_BUTTON_HPP_
