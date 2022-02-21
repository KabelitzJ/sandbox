#include <platform/assert.hpp>

namespace sbx {

template<std::size_t Value, std::size_t Modulo>
requires (std::has_single_bit(Modulo))
inline constexpr std::size_t fast_mod() noexcept {
  return Value & (Modulo - 1);
}

} // namespace sbx