#ifndef LIBSBX_MATH_QUATERNION_HPP_
#define LIBSBX_MATH_QUATERNION_HPP_

#include <cstddef>
#include <concepts>
#include <cmath>
#include <type_traits>

#include <yaml-cpp/yaml.h>

#include <fmt/format.h>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::math {

template<floating_point Type>
class basic_quaternion {

  template<floating_point Other>
  using vector_type_for = basic_vector3<Other>;

  template<floating_point Other>
  using matrix_type_for = basic_matrix4x4<Other>;

  template<floating_point Other>
  using angle_type_for = basic_angle<Other>;

  template<floating_point Other>
  using quaternion_type_for = basic_quaternion<Other>;

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = std::size_t;
  using length_type = std::float_t;
  using vector_type = vector_type_for<value_type>;
  using matrix_type = matrix_type_for<value_type>;
  using angle_type = basic_angle<value_type>;

  inline static constexpr basic_quaternion identity{vector_type::zero, value_type{1}};

  template<floating_point Other = value_type>
  constexpr basic_quaternion(Other value = Other{0}) noexcept;

  template<floating_point Complex = value_type, floating_point Scalar = value_type>
  constexpr basic_quaternion(const vector_type_for<Complex>& complex, Scalar scalar) noexcept;

  template<floating_point Other = value_type>
  constexpr basic_quaternion(const vector_type_for<Other>& euler_angles) noexcept;

  template<floating_point Other = value_type>
  constexpr basic_quaternion(Other x, Other y, Other z, Other w) noexcept;

  template<floating_point Complex = value_type, floating_point Scalar = value_type>
  constexpr basic_quaternion(const vector_type_for<Complex>& axis, const basic_angle<Scalar>& angle) noexcept;

  template<floating_point Other = value_type>
  constexpr basic_quaternion(const matrix_type_for<Other>& matrix) noexcept;

  [[nodiscard]] static constexpr auto dot(const basic_quaternion& lhs, const basic_quaternion& rhs) noexcept -> value_type {
    return vector_type::dot(lhs.complex(), rhs.complex()) + lhs.w() * rhs.w();
  }

  [[nodiscard]] static constexpr auto lerp(const basic_quaternion& start, const basic_quaternion& end, const value_type t) noexcept -> basic_quaternion {
    return start * (1.0f - t) + end * t;
  }

  [[nodiscard]] static constexpr auto slerp(const basic_quaternion& start, basic_quaternion end, const value_type t) noexcept -> basic_quaternion {
    auto dot = basic_quaternion::dot(start, end);

    // Ensure shortest path
    if (dot < 0.0f) {
      end = -end;
      dot = -dot;
    }

    const auto dot_threshold = 0.9995f;

    if (dot > dot_threshold) {
      // If very close, use linear interpolation to avoid divide-by-zero
      auto result = basic_quaternion::lerp(start, end, t);
      return result.normalize();
    }

    const auto theta_0 = std::acos(dot);      // angle between input vectors
    const auto theta = theta_0 * t;      // angle for t
    const auto sin_theta = std::sin(theta);
    const auto sin_theta_0 = std::sin(theta_0);

    const auto s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    const auto s1 = sin_theta / sin_theta_0;

    return start * s0 + end * s1;
  }

  [[nodiscard]] constexpr operator matrix_type() const noexcept;

  [[nodiscard]] constexpr auto to_matrix() const noexcept -> matrix_type;

  template<floating_point Other = value_type>
  constexpr auto operator+=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion&;

  template<floating_point Other = value_type>
  constexpr auto operator-=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion&;

  template<floating_point Other = value_type>
  constexpr auto operator*=(Other value) noexcept -> basic_quaternion&;

  template<floating_point Other = value_type>
  constexpr auto operator*=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion&;

  template<floating_point Other = value_type>
  constexpr auto operator/=(Other value) noexcept -> basic_quaternion&;

  [[nodiscard]] constexpr auto x() noexcept -> reference;

  [[nodiscard]] constexpr auto x() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto y() noexcept -> reference;

  [[nodiscard]] constexpr auto y() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto z() noexcept -> reference;

  [[nodiscard]] constexpr auto z() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto w() noexcept -> reference;

  [[nodiscard]] constexpr auto w() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto complex() noexcept -> vector_type&;

  [[nodiscard]] constexpr auto complex() const noexcept -> const vector_type&;

  [[nodiscard]] constexpr auto scalar() noexcept -> reference;

  [[nodiscard]] constexpr auto scalar() const noexcept -> const_reference;

  [[nodiscard]] constexpr auto length_squared() const noexcept -> length_type;

  [[nodiscard]] constexpr auto length() const noexcept -> length_type;

  constexpr auto normalize() noexcept -> basic_quaternion&;

private:

  vector_type _complex;
  value_type _scalar;

}; // class basic_quaternion

template<floating_point Lhs, floating_point Rhs>
[[nodiscard]] constexpr auto operator==(const basic_quaternion<Lhs>& lhs, const basic_quaternion<Rhs>& rhs) noexcept -> bool;

template<floating_point Lhs, floating_point Rhs>
[[nodiscard]] constexpr auto operator+(basic_quaternion<Lhs> lhs, const basic_quaternion<Rhs>& rhs) noexcept -> basic_quaternion<Lhs>;

template<floating_point Lhs, floating_point Rhs>
[[nodiscard]] constexpr auto operator-(basic_quaternion<Lhs> lhs, const basic_quaternion<Rhs>& rhs) noexcept -> basic_quaternion<Lhs>;

template<floating_point Type>
[[nodiscard]] constexpr auto operator-(basic_quaternion<Type> quaternion) noexcept -> basic_quaternion<Type>;

template<floating_point Lhs, floating_point Rhs>
[[nodiscard]] constexpr auto operator*(basic_quaternion<Lhs> lhs, Rhs scalar) noexcept -> basic_quaternion<Lhs>;

template<floating_point Lhs, floating_point Rhs>
[[nodiscard]] constexpr auto operator*(basic_quaternion<Lhs> lhs, const basic_quaternion<Rhs>& rhs) noexcept -> basic_quaternion<Lhs>;

template<floating_point Lhs, floating_point Rhs>
[[nodiscard]] constexpr auto operator/(basic_quaternion<Lhs> lhs, Rhs scalar) noexcept -> basic_quaternion<Lhs>;

/** @brief Type alias for a quaternion with 32 bit floating-point components. */
using quaternionf = basic_quaternion<std::float_t>;

/** @brief Type alias for quaternionf */
using quaternion = quaternionf;

} // namespace sbx::math

template<sbx::math::floating_point Type>
struct std::hash<sbx::math::basic_quaternion<Type>> {

  auto operator()(const sbx::math::basic_quaternion<Type>& quaternion) const noexcept -> std::size_t;

}; // struct std::hash

template<sbx::math::floating_point Type>
struct YAML::convert<sbx::math::basic_quaternion<Type>> {

  static auto encode(const sbx::math::basic_quaternion<Type>& quaternion) -> YAML::Node;

  static auto decode(const YAML::Node& node, sbx::math::basic_quaternion<Type>& quaternion) -> bool;

}; // struct YAML::convert

template<sbx::math::floating_point Type>
struct fmt::formatter<sbx::math::basic_quaternion<Type>> {

  template<typename ParseContext>
  constexpr auto parse(ParseContext& context) -> decltype(context.begin());

  template<typename FormatContext>
  auto format(const sbx::math::basic_quaternion<Type>& quaternion, FormatContext& context) -> decltype(context.out());

}; // struct fmt::formatter

#include <libsbx/math/quaternion.ipp>

#endif // LIBSBX_MATH_QUATERNION_HPP_
