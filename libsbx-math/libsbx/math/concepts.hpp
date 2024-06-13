#ifndef LIBSBX_MATH_CONCEPTS_HPP_
#define LIBSBX_MATH_CONCEPTS_HPP_

#include <type_traits>
#include <numbers>
#include <limits>
#include <cmath>

namespace sbx::math {

template<typename Type>
concept numeric = (std::is_integral_v<Type> && !std::is_same_v<Type, bool>) || std::is_floating_point_v<Type>;



template<typename Type>
struct is_floating_point : std::bool_constant<std::is_floating_point_v<Type>> { };

template<typename Type>
inline constexpr bool is_floating_point_v = is_floating_point<Type>::value;

template<typename Type>
concept floating_point = is_floating_point_v<Type>;



template<typename Type>
struct is_integral : std::bool_constant<std::is_integral_v<Type> && !std::is_same_v<Type, bool>> { };

template<typename Type>
inline constexpr bool is_integral_v = is_integral<Type>::value;

template<typename Type>
concept integral = is_integral_v<Type>;



template<typename Type>
struct is_scalar : std::bool_constant<is_floating_point_v<Type> || is_integral_v<Type>> { };

template<typename Type>
inline constexpr bool is_scalar_v = is_scalar<Type>::value;

template<typename Type>
concept scalar = is_scalar_v<Type>;



template<typename>
struct comparision_traits;

template<integral Type>
struct comparision_traits<Type> {

  template<scalar Other>
  inline static constexpr auto equal(Type lhs, Other rhs) noexcept -> bool {
    return lhs == static_cast<Type>(rhs);
  }

}; // template<integral Type>

template<floating_point Type>
struct comparision_traits<Type> {

  inline static constexpr auto epsilon = std::numeric_limits<Type>::epsilon();

  template<scalar Other>
  inline static constexpr auto equal(Type lhs, Other rhs) noexcept -> bool {
    return std::abs(lhs - static_cast<Type>(rhs)) <= epsilon;
  }

}; // template<floating_point Type>

} // namespace sbx::math

#endif // LIBSBX_MATH_CONCEPTS_HPP_
