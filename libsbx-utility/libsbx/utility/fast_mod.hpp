#ifndef LIBSBX_UTILITY_FAST_MOD_HPP_
#define LIBSBX_UTILITY_FAST_MOD_HPP_

#include <concepts>

namespace sbx::utility {

template<std::unsigned_integral Type>
constexpr auto fast_mod(const Type value, const Type modulus) noexcept -> Type {
  // return value - (value / modulus) * modulus;
  return value >= modulus ? value % modulus : value;
}

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_FAST_MOD_HPP_
