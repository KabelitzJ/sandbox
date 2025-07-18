#ifndef LIBSBX_MATH_ALGORITHM_HPP_
#define LIBSBX_MATH_ALGORITHM_HPP_

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

template<floating_point Type>
inline constexpr auto mix(const Type x, const Type y, const Type a) -> Type {
  return x * (static_cast<Type>(1) - a) + y * a;
}

} // namespace sbx::math

#endif // LIBSBX_MATH_ALGORITHM_HPP_
