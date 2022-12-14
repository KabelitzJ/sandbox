#ifndef LIBSBX_MATH_CONCEPTS_HPP_
#define LIBSBX_MATH_CONCEPTS_HPP_

#include <type_traits>
#include <concepts>
#include <cinttypes>
#include <cmath>

namespace sbx::math {

template<typename Type>
concept arithmetic = std::is_floating_point_v<Type> || (std::is_integral_v<Type> && !std::is_same_v<Type, bool>);

} // namespace sbx::math

#endif // LIBSBX_MATH_CONCEPTS_HPP_
