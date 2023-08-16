#ifndef LIBSBX_MATH_ANGLE_HPP_
#define LIBSBX_MATH_ANGLE_HPP_

#include <cmath>
#include <concepts>
#include <numbers>

namespace sbx::math {

template<std::floating_point Type>
class basic_degree {

public:

  using value_type = Type;

  constexpr basic_degree() = default;

  explicit constexpr basic_degree(value_type value) noexcept
  : _value{value} {}

  constexpr auto value() const noexcept -> value_type {
    return _value;
  }

  constexpr operator value_type() const noexcept {
    return _value;
  }

private:

  value_type _value{};

}; // class basic_degree

template<std::floating_point Type>
constexpr auto operator==(const basic_degree<Type>& lhs, const basic_degree<Type>& rhs) noexcept -> bool {
  return static_cast<basic_degree<Type>::value_type>(lhs) == static_cast<basic_degree<Type>::value_type>(rhs);
}

template<std::floating_point Type, typename Other>
requires (std::is_convertible_v<Other, Type>)
constexpr auto operator*(const basic_degree<Type>& lhs, const Other rhs) noexcept -> basic_degree<Type> {
  return basic_degree<Type>{lhs.value() * static_cast<Type>(rhs)};
}

using degree = basic_degree<std::float_t>;

template<std::floating_point Type>
class basic_radian {

public:

  using value_type = Type;

  constexpr basic_radian() = default;

  explicit constexpr basic_radian(value_type value) noexcept
  : _value{value} {}

  constexpr auto value() const noexcept -> value_type {
    return _value;
  }

  constexpr operator value_type() const noexcept {
    return _value;
  }

private:

  value_type _value{};

}; // class basic_radian

template<std::floating_point Type>
constexpr auto operator==(const basic_radian<Type>& lhs, const basic_radian<Type>& rhs) noexcept -> bool {
  return static_cast<basic_radian<Type>::value_type>(lhs) == static_cast<basic_radian<Type>::value_type>(rhs);
}

template<std::floating_point Type, typename Other>
requires (std::is_convertible_v<Other, Type>)
constexpr auto operator*(const basic_radian<Type>& lhs, const Other rhs) noexcept -> basic_radian<Type> {
  return basic_radian<Type>{lhs.value() * static_cast<Type>(rhs)};
}

using radian = basic_radian<std::float_t>;

template<std::floating_point Type>
class basic_angle {

public:

  using value_type = Type;

  constexpr basic_angle(const basic_degree<value_type>& degree) noexcept
  : _radian{degree.value() * std::numbers::pi_v<value_type> / static_cast<value_type>(180)} {}

  constexpr basic_angle(const basic_radian<value_type>& radian) noexcept
  : _radian{radian.value()} {}

  constexpr auto to_degrees() const noexcept -> basic_degree<value_type> {
    return basic_degree<value_type>{_radian.value() * static_cast<value_type>(180) / std::numbers::pi_v<value_type>};
  }

  constexpr auto to_radians() const noexcept -> basic_radian<value_type> {
    return _radian;
  }

private:

  basic_radian<Type> _radian{};

}; // class basic_angle

template<std::floating_point Type>
constexpr auto operator==(const basic_angle<Type>& lhs, const basic_angle<Type>& rhs) noexcept -> bool {
  return lhs.to_radians() == rhs.to_radians();
}

using angle = basic_angle<std::float_t>;

template<std::floating_point Type>
constexpr auto to_degrees(const basic_radian<Type>& radian) noexcept -> basic_degree<Type> {
  return basic_angle<Type>{radian}.to_degrees();
}

template<std::floating_point Type>
constexpr auto to_radians(const basic_degree<Type>& degree) noexcept -> basic_radian<Type> {
  return basic_angle<Type>{degree}.to_radians();
}

// [NOTE] KAJ 2023-05-24 : This, somehow, causes a linker error (multiple definition)
// namespace literals {

// auto operator""_deg(long double value) noexcept -> degree {
//   return degree{static_cast<std::float_t>(value)};
// }

// auto operator""_deg(unsigned long long value) noexcept -> degree {
//   return degree{static_cast<std::float_t>(value)};
// }

// auto operator""_rad(long double value) noexcept -> radian {
//   return radian{static_cast<std::float_t>(value)};
// }

// auto operator""_rad(unsigned long long value) noexcept -> radian {
//   return radian{static_cast<std::float_t>(value)};
// }

// } // namespace literals

} // namespace sbx::math

#endif // LIBSBX_MATH_ANGLE_HPP_
