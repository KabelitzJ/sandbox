#ifndef SBX_MATH_ANGLE_HPP_
#define SBX_MATH_ANGLE_HPP_

#include <concepts>

#include <types/primitives.hpp>

#include "constants.hpp"

// [TODO] KAJ 2022-02-09 11:59 - Clean this file up and add functionalities (e.g. comparison operators) to degree, radian, and angle classes

namespace sbx {

/**
 * @brief A class representing an angle in degrees.
 * 
 * @tparam Type The underlying type of the angle.
 */
template<std::floating_point Type>
class degrees {

public:

  /** The underlying type of the angle. */
  using value_type = Type;

  /**
   * @brief Constructs a new degrees object.
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
constexpr bool operator==(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept;

/**
 * @brief Compares two degrees objects
 *
 * @param lhs The left hand side of the comparison.
 * @param rhs The right hand side of the comparison.
 *
 * @return std::strong_ordering The comparison result.
 */
template<std::floating_point Type>
constexpr std::strong_ordering operator<=>(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept;

template<std::floating_point Type>
class radians {

public:

  using value_type = Type;

  explicit constexpr radians(const value_type value);
  ~radians() noexcept = default;

  constexpr operator value_type() const noexcept;

private:

  value_type _value{};

};

template<std::floating_point Type>
constexpr bool operator==(const radians<Type>& lhs, const radians<Type>& rhs) noexcept;

template<std::floating_point Type>
constexpr std::strong_ordering operator<=>(const radians<Type>& lhs, const radians<Type>& rhs) noexcept;

template<std::floating_point Type>
class angle {

public:

  using value_type = Type;

  constexpr angle(const degrees<value_type>& degrees) noexcept;
  constexpr angle(const radians<value_type>& radians) noexcept;
  ~angle() noexcept = default;

  constexpr degrees<value_type> to_degrees() const noexcept;

  constexpr radians<value_type> to_radians() const noexcept;

private:

  value_type _degrees{};

};

template<std::floating_point Type>
constexpr bool operator==(const angle<Type>& lhs, const angle<Type>& rhs) noexcept;

template<std::floating_point Type>
constexpr std::strong_ordering operator<=>(const angle<Type>& lhs, const angle<Type>& rhs) noexcept;

namespace literals {

inline constexpr degrees<float> operator""_degrees(const long double d) {
  return degrees<float>{static_cast<float>(d)};
}

inline constexpr radians<float> operator""_radians(const long double r) {
  return radians<float>{static_cast<float>(r)};
}

} // namespace literals

} // namespace sbx

#include "angle.inl"

#endif // SBX_MATH_ANGLE_HPP_
