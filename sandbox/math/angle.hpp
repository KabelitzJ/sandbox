#ifndef SBX_MATH_ANGLE_HPP_
#define SBX_MATH_ANGLE_HPP_

#include <concepts>

#include <types/primitives.hpp>

#include "constants.hpp"

// [TODO] KAJ 2022-02-09 11:59 - Clean this file up and add functionalities (e.g. comparison operators) to degree, radian, and angle classes

namespace sbx {

/**
 * @brief Represents an angle in degrees.
 * 
 * @tparam Type The underlying type of the angle.
 */
template<std::floating_point Type>
class degrees {

public:

  /** The underlying type of the angle. */
  using value_type = Type;

  /**
   * @brief Constructs a new degrees object with the given value.
   *
   * @param value The value of the angle in degrees.
   */
  explicit constexpr degrees(const value_type value);

  /** @brief Destroys the degrees object. */
  ~degrees() noexcept = default;

  /**
   * @brief Converts the degrees object to its underlying type.
   *
   * @return value_type The underlying type of the degrees object.
   */
  constexpr operator value_type() const noexcept;

private:

  value_type _value{};

};

/**
 * @brief Compares two degrees objects for equality.   
 * 
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
 * 
 * @return bool True if the two degrees objects are equal, false otherwise.
 */
template<std::floating_point Type>
[[nodiscard]] constexpr bool operator==(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept;

/**
 * @brief Compares two degrees objects
 *
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
 *
 * @return std::strong_ordering The comparison result.
 */
template<std::floating_point Type>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept;


/**
 * @brief Represents an angle in radians.
 * 
 * @tparam Type The underlying type of the angle.
 */
template<std::floating_point Type>
class radians {

public:

  /** @brief The underlying type of the angle. */
  using value_type = Type;

  /**
   * @brief Constructs a new radians object with the given value.
   * 
   * @param value The value of the angle in radians.
   */
  explicit constexpr radians(const value_type value);

  /** @brief Destroys the radians object. */
  ~radians() noexcept = default;

  /**
   * @brief Converts the radians object to its underlying type.
   * 
   * @return value_type The value of the radians object.
   */
  constexpr operator value_type() const noexcept;

private:

  value_type _value{};

};

/**
 * @brief Compares two radians objects for equality.
 * 
 * @tparam Type The underlying type of the angle.
 * 
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
 * @return bool True if the two radians objects are equal, false otherwise. 
 */
template<std::floating_point Type>
[[nodiscard]] constexpr bool operator==(const radians<Type>& lhs, const radians<Type>& rhs) noexcept;

/**
 * @brief Compares two radians objects.
 * 
 * @tparam Type The underlying type of the angle.
 * 
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
 * 
 * @return std::strong_ordering The comparison result.
 */
template<std::floating_point Type>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const radians<Type>& lhs, const radians<Type>& rhs) noexcept;

/**
 * @brief Represents an angle.
 * 
 * @tparam Type The underlying type of the angle.
 */
template<std::floating_point Type>
class angle {

public:

  /** @brief The underlying type of the angle. */
  using value_type = Type;

  /**
   * @brief Constructs a new angle object with degrees.
   * 
   * @param degrees The value of the angle in degrees.
   */
  constexpr angle(const degrees<value_type>& degrees) noexcept;

  /**
   * @brief Constructs a new angle object with radians.
   * 
   * @param radians The value of the angle.
   */
  constexpr angle(const radians<value_type>& radians) noexcept;

  /** @brief Destroys the angle object. */
  ~angle() noexcept = default;

  /**
   * @brief Converts the angle to degrees.
   * 
   * @return degrees<value_type> The value of the angle in degrees. 
   */
  constexpr degrees<value_type> to_degrees() const noexcept;

  /**
   * @brief Converts the angle to radians.
   * 
   * @return radians<value_type> The value of the angle in radians. 
   */
  constexpr radians<value_type> to_radians() const noexcept;

private:

  value_type _degrees{};

};

/**
 * @brief Compares two angles for equality.
 * 
 * @tparam Type The underlying type of the angle.
 * 
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
 * 
 * @return bool True if the two angles are equal, false otherwise. 
 */
template<std::floating_point Type>
[[nodiscard]] constexpr bool operator==(const angle<Type>& lhs, const angle<Type>& rhs) noexcept;

/**
 * @brief Compares two angles.
 * 
 * @tparam Type The underlying type of the angle.
 * 
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
 * 
 * @return std::strong_ordering The comparison result.
 */
template<std::floating_point Type>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const angle<Type>& lhs, const angle<Type>& rhs) noexcept;

namespace literals {

/**
 * @brief Creates a new degrees object from a floating point literal.
 * 
 * @param degrees The value of the degrees object.
 * 
 * @return degrees<float> The degrees object that represents the given value. 
 */
constexpr degrees<float32> operator""_degrees(const long double degrees);

/**
 * @brief Creates a new radians object from a floating point literal.
 * 
 * @param radians The value of the radians object.
 *  
 * @return radians<float> The radians object that represents the given value.
 */
constexpr radians<float32> operator""_radians(const long double radians);

} // namespace literals

} // namespace sbx

#include "angle.inl"

#endif // SBX_MATH_ANGLE_HPP_
