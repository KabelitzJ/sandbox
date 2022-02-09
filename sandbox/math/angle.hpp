#ifndef SBX_MATH_ANGLE_HPP_
#define SBX_MATH_ANGLE_HPP_

#include <concepts>

#include <types/primitives.hpp>

#include "constants.hpp"

// [TODO] KAJ 2022-02-09 11:59 - Clean this file up and add functionalities (e.g. comparison operators) to degree, radian, and angle classes

namespace sbx {

template<typename Type>
requires std::floating_point<Type>
class degrees {

public:

  using value_type = Type;

  explicit constexpr degrees(const value_type value) : _value{value} {}
  ~degrees() noexcept = default;

  constexpr operator value_type() const noexcept {
    return _value;
  } 

private:

  value_type _value{};

};

template<typename Type>
requires std::floating_point<Type>
constexpr bool operator==(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept;

template<typename Type>
requires std::floating_point<Type>  
constexpr std::strong_ordering operator<=>(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept;

template<typename Type>
requires std::floating_point<Type>
class radians {

public:

  using value_type = Type;

  explicit constexpr radians(const value_type value) : _value{value} {}
  ~radians() noexcept = default;

  constexpr operator value_type() const noexcept {
    return _value;
  } 

private:

  value_type _value{};

};

template<typename Type>
requires std::floating_point<Type>
constexpr bool operator==(const radians<Type>& lhs, const radians<Type>& rhs) noexcept;

template<typename Type>
requires std::floating_point<Type>
constexpr std::strong_ordering operator<=>(const radians<Type>& lhs, const radians<Type>& rhs) noexcept;

template<typename Type>
requires std::floating_point<Type>
class angle {

public:

  using value_type = Type;

  constexpr angle(const degrees<value_type>& degrees) noexcept : _degrees{degrees} { }
  constexpr angle(const radians<value_type>& radians) noexcept : _degrees{radians * value_type{180.0} / pi_v<value_type>} { }
  ~angle() noexcept = default;

  constexpr degrees<value_type> to_degrees() const noexcept {
    return degrees<value_type>{_degrees};
  }

  constexpr radians<value_type> to_radians() const noexcept {
    return radians<value_type>{_degrees * pi_v<value_type> / value_type{180.0}};
  }

private:

  value_type _degrees{};

};

template<typename Type>
requires std::floating_point<Type>
constexpr bool operator==(const angle<Type>& lhs, const angle<Type>& rhs) noexcept;

template<typename Type>
requires std::floating_point<Type>
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
