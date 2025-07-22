#ifndef LIBSBX_MATH_ALGORITHM_HPP_
#define LIBSBX_MATH_ALGORITHM_HPP_

#include <cmath>

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

template<floating_point Type>
inline constexpr auto mix(const Type x, const Type y, const Type a) -> Type {
  return x * (static_cast<Type>(1) - a) + y * a;
}

template<floating_point Type>
inline constexpr auto abs(const Type value) -> Type {
  return std::abs(value);
}

} // namespace sbx::math

#endif // LIBSBX_MATH_ALGORITHM_HPP_
