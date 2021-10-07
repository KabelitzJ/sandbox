#ifndef SBX_UTIL_MEMORY_HPP_
#define SBX_UTIL_MEMORY_HPP_

#include <cstddef>
#include <utility>

namespace sbx {

[[nodiscard]] inline constexpr bool is_power_of_two(const std::size_t value) noexcept {
  return value && ((value & (value - 1u)) == 0);
}

template<std::size_t Value>
[[nodiscard]] constexpr std::size_t fast_mod(const std::size_t value) noexcept {
  static_assert(is_power_of_two(Value), "Value must be a power of two");
  return value & (Value - 1u);
}

template<typename Type>
[[nodiscard]] constexpr auto to_address(Type&& pointer) noexcept {
  if constexpr(std::is_pointer_v<std::remove_const_t<std::remove_reference_t<Type>>>) {
    return pointer;
  } else {
    return to_address(std::forward<Type>(pointer).operator->());
  }
}

} // namespace sbx

#endif // SBX_UTIL_MEMORY_HPP_
