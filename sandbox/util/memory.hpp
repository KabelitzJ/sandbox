#ifndef SBX_UTIL_MEMORY_HPP_
#define SBX_UTIL_MEMORY_HPP_

#include <cstddef>

namespace sbx {

[[nodiscard]] inline constexpr bool is_power_of_two(const std::size_t value) noexcept {
  return value && ((value & (value - 1u)) == 0);
}

template<std::size_t Value>
[[nodiscard]] constexpr std::size_t fast_mod(const std::size_t value) noexcept {
  static_assert(is_power_of_two(Value), "Value must be a power of two");
  return value & (Value - 1u);
}

} // namespace sbx

#endif // SBX_UTIL_MEMORY_HPP_
