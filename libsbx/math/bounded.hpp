// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_MATH_BOUNDED_HPP_
#define LIBSBX_MATH_BOUNDED_HPP_

#include <concepts>
#include <limits>
#include <type_traits>
#include <compare>

#include <libsbx/utility/target.hpp>
#include <libsbx/utility/assert.hpp>

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

template<typename Type, Type Min, Type Max>
requires (std::is_arithmetic_v<Type> && Min <= Max)
class bounded {

public:

  using value_type = Type;

  inline static constexpr auto min = Min;
  inline static constexpr auto max = Max;

  constexpr bounded() requires (value_type{0} >= min && value_type{0} <= max)
  : _value{value_type{0}} { }

  constexpr bounded() requires (min > value_type{0})
  : _value{min} { }

  constexpr bounded() requires (max < value_type{0})
  : _value{max} { }

  constexpr explicit bounded(const value_type value)
  : _value{value} {
    utility::assert_that(value >= min && value <= max, "Invalid value");
  }


  constexpr auto operator=(const value_type value) -> bounded& {
    utility::assert_that(value >= min && value <= max, "Invalid value");

    _value = value;

    return *this;
  }

  [[nodiscard]] constexpr value_type value() const noexcept {
    return _value;
  }

  constexpr explicit operator value_type() const noexcept {
    return _value;
  }

  constexpr auto operator<=>(const bounded& other) const noexcept {
    return _value <=> other._value;
  }

  constexpr bool operator==(const bounded& other) const noexcept {
    return _value == other._value;
  }

  constexpr auto operator<=>(const value_type other) const noexcept {
    return _value <=> other;
  }

  constexpr bool operator==(const value_type other) const noexcept {
    return _value == other;
  }

  friend constexpr auto operator<=>(const value_type lhs, const bounded& rhs) noexcept {
    return lhs <=> rhs._value;
  }

  friend constexpr bool operator==(const value_type lhs, const bounded& rhs) noexcept {
    return lhs == rhs._value;
  }


private:

  value_type _value;

}; // class bounded

} // namespace sbx::math

#endif // LIBSBX_MATH_BOUNDED_HPP_
