#ifndef LIBSBX_MATH_ALGORITHM_HPP_
#define LIBSBX_MATH_ALGORITHM_HPP_

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

template<floating_point Type>
inline constexpr auto mix(const Type start, const Type end, const Type factor) -> Type {
  return start * (static_cast<Type>(1) - factor) + end * factor;
}

} // namespace sbx::math

#endif // LIBSBX_MATH_ALGORITHM_HPP_
