#ifndef SBX_MATH_VECTOR2_HPP_
#define SBX_MATH_VECTOR2_HPP_

#include <ostream>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

/**
 * @brief A vector in two-dimensional space.
 *
 * @tparam Type The type of the vectors components.
 */
template<typename Type>
struct basic_vector2 {

  // Vector components can only be arithmetic types.
  static_assert(std::is_arithmetic_v<Type>, "Type must be arithmetic");

  // -- Type aliases --

  /** @brief The type of the vector components. */
  using value_type = Type;

  /** @brief The that can describe the length of the vector */
  using length_type = float32;

  // -- Static data members --

  /** @brief The origin of two-dimensional space */
  inline static constexpr basic_vector2<value_type> origin{value_type{0}, value_type{0}};

  /** @brief A unit vector along the positive x-axis */
  inline static constexpr basic_vector2<value_type> right{value_type{1}, value_type{0}};

  /** @brief A unit vector along the negative x-axis */
  inline static constexpr basic_vector2<value_type> left{value_type{-1}, value_type{0}};

  /** @brief A unit vector along the positive y-axis */
  inline static constexpr basic_vector2<value_type> up{value_type{0}, value_type{1}};

  /** @brief A unit vector along the negative y-axis */
  inline static constexpr basic_vector2<value_type> down{value_type{0}, value_type{-1}};

  // -- Data members --

  /** @brief The x-component of the vector. */
  value_type x{};
  /** @brief The y-component of the vector. */
  value_type y{};

  // -- Constructors --

  constexpr basic_vector2() noexcept;

  constexpr basic_vector2(const basic_vector2&) noexcept = default;

  constexpr basic_vector2(basic_vector2&&) noexcept = default;

  ~basic_vector2() noexcept = default;

}; // struct basic_vector2

using vector2f = basic_vector2<float32>;

using vector2i = basic_vector2<int32>;

using vector2 = vector2f;

} // namespace sbx

#include "vector2_inl.hpp"

#endif // SBX_MATH_VECTOR2_HPP_
