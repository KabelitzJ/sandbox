#ifndef LIBSBX_MATH_QUATERNION_HPP_
#define LIBSBX_MATH_QUATERNION_HPP_

#include <cstddef>
#include <concepts>
#include <cmath>
#include <type_traits>

#include <libsbx/math/angle.hpp>
#include <libsbx/math/vector3.hpp>

namespace sbx::math {

/**
 * @brief A quaternion that represents a rotation.
 * 
 * @tparam Type The type of the quaternion components.
 */
template<std::floating_point Type>
class basic_quaternion {

public:

  using value_type = Type;

  using length_type = std::float_t;

  inline static constexpr basic_quaternion identity{basic_vector3<value_type>::zero, basic_radian<value_type>{static_cast<value_type>(1)}};

  /**
    * @brief Construct a new quaternion.
    * 
    * @param axis The axis of rotation.
    * @param angle The angle of rotation.
    */
  constexpr basic_quaternion(const basic_vector3<value_type>& axis, const basic_angle<value_type>& angle) noexcept;

  /**
   * @brief Convert a quaternion of a different type.
   * 
   * @tparam Other The type of the other quaternion components.
   * 
   * @param other The other quaternion.
   */
  template<std::floating_point Other>
  requires (std::is_convertible_v<Other, value_type>)
  constexpr basic_quaternion(const basic_quaternion<Other>& other) noexcept;

  /**
   * @brief Copy constructor.
   * 
   * @param other The other quaternion.
   */
  constexpr basic_quaternion(const basic_quaternion& other) noexcept = default;

  /**
   * @brief Move constructor.
   * 
   * @param other The other quaternion.
   */
  constexpr basic_quaternion(basic_quaternion&& other) noexcept = default;

  /**
   * @brief Destroy the quaternion.
   */
  ~basic_quaternion() noexcept = default;

  /**
   * @brief Calculate the conjugate of a quaternion.
   * 
   * @param quaternion The quaternion.
   * 
   * @return basic_quaternion<value_type> The conjugated quaternion.
   */
  [[nodiscard]] static constexpr auto conjugated(const basic_quaternion<value_type>& quaternion) noexcept -> basic_quaternion<value_type>;

  /**
   * @brief Normalize a quaternion.
   * 
   * @param quaternion The quaternion.
   * 
   * @return basic_quaternion<value_type> The normalized quaternion.
   */
  [[nodiscard]] static constexpr auto normalized(const basic_quaternion<value_type>& quaternion) noexcept -> basic_quaternion<value_type>;

  /**
   * @brief Invert a quaternion.
   * 
   * @param quaternion The quaternion.
   * 
   * @return basic_quaternion<value_type> The inverted quaternion.
   */
  [[nodiscard]] static constexpr auto inverted(const basic_quaternion<value_type>& quaternion) noexcept -> basic_quaternion<value_type>;

  /**
   * @brief Copy assignment operator.
   * 
   * @param other The other quaternion.
   * 
   * @return basic_quaternion& A reference to this quaternion.
   */
  constexpr auto operator=(const basic_quaternion& other) noexcept -> basic_quaternion& = default;

  /**
   * @brief Move assignment operator.
   * 
   * @param other The other quaternion.
   * 
   * @return basic_quaternion& A reference to this quaternion.
   */
  constexpr auto operator=(basic_quaternion&& other) noexcept -> basic_quaternion& = default;

  /**
   * @brief Add a quaternion to this quaternion.
   * 
   * @param other The other quaternion.
   * 
   * @return basic_quaternion& A reference to this quaternion.
   */
  template<std::floating_point Other>
  requires (std::is_convertible_v<Other, value_type>)
  constexpr auto operator+=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion&;

  /**
   * @brief Subtract a quaternion from this quaternion.
   * 
   * @param other The other quaternion.
   * 
   * @return basic_quaternion& A reference to this quaternion.
   */
  template<std::floating_point Other>
  requires (std::is_convertible_v<Other, value_type>)
  constexpr auto operator-=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion&;

  /**
   * @brief Multiply this quaternion by another quaternion.
   * 
   * @tparam Other The type of the other quaternion components.
   * 
   * @param other The other quaternion.
   * 
   * @return basic_quaternion& A reference to this quaternion.
   */
  template<std::floating_point Other>
  requires (std::is_convertible_v<Other, value_type>)
  constexpr auto operator*=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion&;

  /**
   * @brief Get the axis of rotation.
   * 
   * @return basic_vector3<value_type> The axis of rotation.
   */
  [[nodiscard]] constexpr auto axis() const noexcept -> const basic_vector3<value_type>&;

  /**
   * @brief Get the angle of rotation.
   * 
   * @return basic_angle<value_type> The angle of rotation.
   */
  [[nodiscard]] constexpr auto angle() const noexcept -> const basic_angle<value_type>&;

  [[nodiscard]] constexpr auto length_squared() const noexcept -> length_type;

  [[nodiscard]] constexpr auto length() const noexcept -> length_type;

private:

  basic_vector3<value_type> _axis;
  basic_angle<value_type> _angle;

}; // class basic_quaternion

/**
 * @brief Compare two quaternions for equality.
 * 
 * @tparam LhsType The type of the left-hand side quaternion components.
 * @tparam RhsType The type of the right-hand side quaternion components.
 * 
 * @param lhs The left-hand side quaternion.
 * @param rhs The right-hand side quaternion.
 * 
 * @return true The quaternions are equal.
 */
template<std::floating_point Type>
constexpr auto operator==(const basic_quaternion<Type>& lhs, const basic_quaternion<Type>& rhs) noexcept -> bool;

/**
 * @brief Add two quaternions.
 * 
 * @tparam LhsType The type of the left-hand side quaternion components.
 * @tparam RhsType The type of the right-hand side quaternion components.
 * 
 * @param lhs The left-hand side quaternion.
 * @param rhs The right-hand side quaternion.
 * 
 * @return basic_quaternion<value_type> The result of the addition.
 */
template<std::floating_point LhsType, std::floating_point RhsType>
requires (std::is_convertible_v<RhsType, LhsType>)
constexpr auto operator+(basic_quaternion<LhsType> lhs, const basic_quaternion<RhsType>& rhs) noexcept -> basic_quaternion<LhsType>;

/**
 * @brief Subtract two quaternions.
 * 
 * @tparam LhsType The type of the left-hand side quaternion components.
 * @tparam RhsType The type of the right-hand side quaternion components.
 * 
 * @param lhs The left-hand side quaternion.
 * @param rhs The right-hand side quaternion.
 * 
 * @return basic_quaternion<LhsType> The result of the subtraction.
 */
template<std::floating_point LhsType, std::floating_point RhsType>
requires (std::is_convertible_v<RhsType, LhsType>)
constexpr auto operator-(basic_quaternion<LhsType> lhs, const basic_quaternion<RhsType>& rhs) noexcept -> basic_quaternion<LhsType>;

/**
 * @brief Multiply two quaternions.
 * 
 * @tparam LhsType The type of the left-hand side quaternion components.
 * @tparam RhsType The type of the right-hand side quaternion components.
 * 
 * @param lhs The left-hand side quaternion.
 * @param rhs The right-hand side quaternion.
 * 
 * @return basic_quaternion<LhsType> The result of the multiplication.
 */
template<std::floating_point LhsType, std::floating_point RhsType>
requires (std::is_convertible_v<RhsType, LhsType>)
constexpr auto operator*(basic_quaternion<LhsType> lhs, const basic_quaternion<RhsType>& rhs) noexcept -> basic_quaternion<LhsType>;

using quaternionf = sbx::math::basic_quaternion<std::float_t>;

using quaternion = quaternionf;

} // namespace sbx::math

#include <libsbx/math/quaternion.ipp>

#endif // LIBSBX_MATH_QUATERNION_HPP_
