#include <cassert>
#include <cmath>

#include <libsbx/utility/hash.hpp>

namespace sbx::math {

template<std::floating_point Type>
inline constexpr basic_quaternion<Type>::basic_quaternion(const basic_vector3<Type>& axis, const basic_angle<Type>& angle) noexcept
: _axis{axis},
  _angle{angle} { }

template<std::floating_point Type>
template<std::floating_point Other>
requires (std::is_convertible_v<Other, Type>)
inline constexpr basic_quaternion<Type>::basic_quaternion(const basic_quaternion<Other>& other) noexcept
: _axis{other._axis},
  _angle{other._angle} { }

template<std::floating_point Type>
inline constexpr auto basic_quaternion<Type>::conjugated(const basic_quaternion<Type>& quaternion) noexcept -> basic_quaternion<Type> {
  return basic_quaternion{-quaternion._axis, quaternion._angle};
}

template<std::floating_point Type>
inline constexpr auto basic_quaternion<Type>::normalized(const basic_quaternion<Type>& quaternion) noexcept -> basic_quaternion<Type> {
  const auto length = quaternion.length();

  if (length != static_cast<length_type>(0)) {
    return basic_quaternion{quaternion._axis * 1 / length, quaternion._angle * 1 / length};
  }

  return quaternion;
}

template<std::floating_point Type>
inline constexpr auto basic_quaternion<Type>::inverted(const basic_quaternion<Type>& quaternion) noexcept -> basic_quaternion<Type> {
  const auto length_squared = quaternion.length_squared();

  if (length_squared != static_cast<length_type>(0)) {
    const auto conjugate = conjugated(quaternion);

    return basic_quaternion{conjugate._axis * 1 / length_squared, conjugate._angle * 1 / length_squared};
  }

  return quaternion;
}

template<std::floating_point Type>
template<std::floating_point Other>
requires (std::is_convertible_v<Other, Type>)
inline constexpr auto basic_quaternion<Type>::operator+=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion<Type>& {
  _axis += basic_vector3<Type>{other._axis};
  _angle += basic_angle<Type>{other._angle};

  return *this;
}

template<std::floating_point Type>
template<std::floating_point Other>
requires (std::is_convertible_v<Other, Type>)
inline constexpr auto basic_quaternion<Type>::operator-=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion<Type>& {
  _axis -= basic_vector3<Type>{other._axis};
  _angle -= basic_angle<Type>{other._angle};

  return *this;
}

template<std::floating_point Type>
template<std::floating_point Other>
requires (std::is_convertible_v<Other, Type>)
inline constexpr auto basic_quaternion<Type>::operator*=(const basic_quaternion<Other>& other) noexcept -> basic_quaternion<Type>& {
  const auto this_angle = static_cast<value_type>(_angle.to_radians());
  const auto other_angle = static_cast<value_type>(other._angle.to_radians());

  const auto angle = this_angle * other_angle - basic_vector3<value_type>::dot(_axis, other._axis);
  const auto axis =  other._axis * this_angle + _axis * other_angle + basic_vector3<value_type>::cross(_axis, other._axis);

  return (*this = basic_quaternion{axis, basic_radian<value_type>{angle}});
}

template<std::floating_point Type>
inline constexpr auto basic_quaternion<Type>::axis() const noexcept -> const basic_vector3<Type>& {
  return _axis;
}

template<std::floating_point Type>
inline constexpr auto basic_quaternion<Type>::angle() const noexcept -> const basic_angle<Type>& {
  return _angle;
}

template<std::floating_point Type>
inline constexpr auto basic_quaternion<Type>::length_squared() const noexcept -> length_type {
  const auto angle = static_cast<value_type>(_angle.to_radians());

  return _axis.length_squared() + angle * angle;
}

template<std::floating_point Type>
inline constexpr auto basic_quaternion<Type>::length() const noexcept -> length_type {
  return std::sqrt(length_squared());
}

template<std::floating_point LhsType, std::floating_point RhsType>	
requires (std::is_convertible_v<RhsType, LhsType>)
constexpr auto operator==(const basic_quaternion<LhsType>& lhs, const basic_quaternion<RhsType>& rhs) noexcept -> bool {
  return lhs.axis() == rhs.axis() && lhs.angle() == rhs.angle();
}

template<std::floating_point LhsType, std::floating_point RhsType>
requires (std::is_convertible_v<RhsType, LhsType>)
constexpr auto operator+(basic_quaternion<LhsType> lhs, const basic_quaternion<RhsType>& rhs) noexcept -> basic_quaternion<LhsType> {
  return lhs += rhs;
}

template<std::floating_point LhsType, std::floating_point RhsType>
requires (std::is_convertible_v<RhsType, LhsType>)
constexpr auto operator-(basic_quaternion<LhsType> lhs, const basic_quaternion<RhsType>& rhs) noexcept -> basic_quaternion<LhsType> {
  return lhs -= rhs;
}

template<std::floating_point LhsType, std::floating_point RhsType>
requires (std::is_convertible_v<RhsType, LhsType>)
constexpr auto operator*(basic_quaternion<LhsType> lhs, const basic_quaternion<RhsType>& rhs) noexcept -> basic_quaternion<LhsType> {
  return lhs *= rhs;
}

} // namespace sbx::math
