#include <platform/assert.hpp>

namespace sbx {

template<std::unsigned_integral Type>
inline constexpr bool is_power_of_two(const Type value) noexcept {
  return (value & (value - 1)) == 0;
}

template<std::unsigned_integral Type>
inline constexpr Type fast_mod(const Type value, const Type modulo) noexcept {
  SBX_ASSERT(is_power_of_two(modulo), "Value must be a power of two");
  return value & (modulo - 1);
}

} // namespace sbx