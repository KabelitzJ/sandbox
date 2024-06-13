#ifndef LIBSBX_UTILITY_FAST_MOD_HPP_
#define LIBSBX_UTILITY_FAST_MOD_HPP_

#include <concepts>
#include <cmath>

namespace sbx::utility {

/**
 * @brief Fast modulus operation. If the value is less than the modulus, the value can be returned directly.
 * 
 * @tparam Type The type of the value and modulus.
 * 
 * @param value The value to be modded.
 * @param modulus The modulus to be used.
 * 
 * @return Type The result of the modulus operation.
 */
template<std::unsigned_integral Type>
constexpr auto fast_mod(const Type value, const Type modulus) noexcept -> Type {
  // return value - (value / modulus) * modulus;
  return value < modulus ? value : value % modulus;
}

template<std::floating_point Type>
constexpr auto fast_mod(const Type value, const Type modulus) noexcept -> Type {
  // return value - (value / modulus) * modulus;
  return value < modulus ? value : std::fmod(value, modulus);
}

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_FAST_MOD_HPP_
