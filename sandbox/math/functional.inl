#include <platform/assert.hpp>

namespace sbx {

template<std::unsigned_integral Type>
inline constexpr Type fast_mod(const Type value, const Type modulo) noexcept {
  SBX_ASSERT(std::has_single_bit(modulo), "Modulo must be a power of 2");
  return value & (modulo - 1);
}

} // namespace sbx