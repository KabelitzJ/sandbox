#ifndef LIBSBX_MATH_QUATERNION_HPP_
#define LIBSBX_MATH_QUATERNION_HPP_

#include <cstddef>
#include <concepts>
#include <cmath>
#include <type_traits>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector3.hpp>

namespace sbx::math {

template<scalar Type>
class basic_quaternion {

  template<scalar Other>
  using vector_type_for = basic_vector3<Other>;

  template<scalar Other>
  using matrix_type_for = basic_matrix4x4<Other>;

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = std::size_t;
  using length_type = std::float_t;
  using vector_type = vector_type_for<value_type>;
  using matrix_type = matrix_type_for<value_type>;

  template<scalar Other = value_type>
  constexpr basic_quaternion(Other value = Other{0}) noexcept;

  template<scalar Other = value_type>
  constexpr basic_quaternion(const vector_type_for<Other>& complex, Other scalar = Other{0}) noexcept;

  template<scalar Other = value_type>
  constexpr basic_quaternion(Other x, Other y, Other z, Other w) noexcept;

  constexpr auto complex() noexcept -> vector_type&;

  constexpr auto complex() const noexcept -> const vector_type&;

  constexpr auto scalar() noexcept -> reference;

  constexpr auto scalar() const noexcept -> const_reference;

private:

  vector_type _complex;
  value_type _scalar;

}; // class basic_quaternion

/** @brief Type alias for a quaternion with 32 bit floating-point components. */
using quaternionf = basic_quaternion<std::float_t>;

/** @brief Type alias for quaternionf */
using quaternion = quaternionf;

} // namespace sbx::math

template<sbx::math::scalar Type>
struct std::hash<sbx::math::basic_quaternion<Type>> {

  auto operator()(const sbx::math::basic_quaternion<Type>& quaternion) const noexcept -> std::size_t;

}; // struct std::hash

#include <libsbx/math/quaternion.ipp>

#endif // LIBSBX_MATH_QUATERNION_HPP_
