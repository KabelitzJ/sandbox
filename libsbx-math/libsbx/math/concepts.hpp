#ifndef LIBSBX_MATH_CONCEPTS_HPP_
#define LIBSBX_MATH_CONCEPTS_HPP_

#include <type_traits>
#include <concepts>

namespace sbx::math {

template<typename Type>
concept numeric = (std::is_integral_v<Type> && !std::is_same_v<Type, bool>) || std::is_floating_point_v<Type>;

template<typename Type>
concept addable = requires(Type a, Type b) {
  { a + b } -> std::convertible_to<Type>;
  { a += b } -> std::convertible_to<Type&>;
}; // concept addable

template<typename Type>
concept subtractable = requires(Type a, Type b) {
  { a - b } -> std::convertible_to<Type>;
  { a -= b } -> std::convertible_to<Type&>;
}; // concept subtractable

template<typename Type, typename Scalar = Type>
concept multipliable = requires(Type a, Scalar b) {
  { a * b } -> std::convertible_to<Type>;
  { a *= b } -> std::convertible_to<Type&>;
}; // concept multipliable

template<typename Type, typename Scalar = Type>
concept dividable = requires(Type a, Scalar b) {
  { a / b } -> std::convertible_to<Type>;
  { a /= b } -> std::convertible_to<Type&>;
}; // concept dividable

template<typename Type>
concept comparable = requires(Type a, Type b) {
  { a == b } -> std::convertible_to<bool>;
  { a != b } -> std::convertible_to<bool>;
  { a < b } -> std::convertible_to<bool>;
  { a > b } -> std::convertible_to<bool>;
  { a <= b } -> std::convertible_to<bool>;
  { a >= b } -> std::convertible_to<bool>;
}; // concept comparable

template<typename Type>
concept arithmetic = addable<Type> && subtractable<Type> && multipliable<Type> && dividable<Type> && comparable<Type>;

} // namespace sbx::math

#endif // LIBSBX_MATH_CONCEPTS_HPP_
