#include <platform/assert.hpp>

#include <bit>

namespace sbx {

template<std::unsigned_integral Type>
inline constexpr Type fast_mod(const Type value, const Type modulo) noexcept {
  SBX_ASSERT(std::has_single_bit(modulo), "Modulo must be a power of two.");
  return value > 0 && value & (modulo - 1);
}

} // namespace sbx