#ifndef SBX_MATH_ANGLE_HPP_
#define SBX_MATH_ANGLE_HPP_

#include <meta/type_guards.hpp>

#include "constants.hpp"

namespace sbx {

template<typename Type, IS_FLOATING_POINT(Type)>
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

template<typename Type, IS_FLOATING_POINT(Type)>
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

template<typename Type, IS_FLOATING_POINT(Type)>
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

namespace literals {

inline constexpr angle<float> operator""_degrees(const long double d) {
  return angle<float>{degrees<float>{static_cast<float>(d)}};
}

inline constexpr angle<float> operator""_radians(const long double r) {
  return angle<float>{radians<float>{static_cast<float>(r)}};
}

} // namespace literals

} // namespace sbx

#include "angle.inl"

#endif // SBX_MATH_ANGLE_HPP_
