#ifndef LIBSBX_MATH_CONSTANTS_HPP_
#define LIBSBX_MATH_CONSTANTS_HPP_

#include <cstdint>
#include <limits>
#include <numbers>

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

template<floating_point Type>
inline constexpr auto epsilon_v = std::numeric_limits<Type>::epsilon();

inline constexpr auto epsilonf = std::numeric_limits<std::float_t>::epsilon();

inline constexpr auto epsilond = std::numeric_limits<std::double_t>::epsilon();

} // namespace sbx::math

#endif // LIBSBX_MATH_CONSTANTS_HPP_
