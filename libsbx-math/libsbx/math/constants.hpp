#ifndef LIBSBX_MATH_CONSTANTS_HPP_
#define LIBSBX_MATH_CONSTANTS_HPP_

#include <cstdint>
#include <cmath>
#include <limits>
#include <numbers>

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

template<floating_point Type>
inline constexpr auto epsilon_v = std::numeric_limits<Type>::epsilon();

inline constexpr auto epsilonf = epsilon_v<std::float_t>;

inline constexpr auto epsilond = epsilon_v<std::double_t>;

} // namespace sbx::math

#endif // LIBSBX_MATH_CONSTANTS_HPP_
