namespace sbx {

template<std::floating_point Type>
inline constexpr degrees<Type>::degrees(const value_type value)
: _value{value} { }

template<std::floating_point Type>
inline constexpr degrees<Type>::operator Type() const noexcept {
  return _value;
}

template<std::floating_point Type>
inline constexpr bool operator==(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept {
  return static_cast<Type>(lhs) == static_cast<Type>(rhs);
}

template<std::floating_point Type>
inline constexpr std::strong_ordering operator<=>(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept {
  return static_cast<Type>(lhs) <=> static_cast<Type>(rhs);
}

template<std::floating_point Type>
inline constexpr radians<Type>::radians(const value_type value)
: _value{value} { }

template<std::floating_point Type>
inline constexpr radians<Type>::operator Type() const noexcept {
  return _value;
}

template<std::floating_point Type>
inline constexpr bool operator==(const radians<Type>& lhs, const radians<Type>& rhs) noexcept {
  return static_cast<Type>(lhs) == static_cast<Type>(rhs);
}

template<std::floating_point Type>
inline constexpr std::strong_ordering operator<=>(const radians<Type>& lhs, const radians<Type>& rhs) noexcept {
  return static_cast<Type>(lhs) <=> static_cast<Type>(rhs);
}

template<std::floating_point Type>
inline constexpr angle<Type>::angle(const degrees<value_type>& degrees) noexcept
: _degrees{degrees} { }

template<std::floating_point Type>
inline constexpr angle<Type>::angle(const radians<value_type>& radians) noexcept
: _degrees{radians * value_type{180} / pi_v<value_type>} { }

template<std::floating_point Type>
inline constexpr degrees<Type> angle<Type>::to_degrees() const noexcept {
  return degrees<value_type>{_degrees};
}

template<std::floating_point Type>
inline constexpr radians<Type> angle<Type>::to_radians() const noexcept {
  return radians<value_type>{_degrees * pi_v<value_type> / value_type{180.0}};
}

template<std::floating_point Type>
inline constexpr bool operator==(const angle<Type>& lhs, const angle<Type>& rhs) noexcept {
  return lhs.to_degrees() == rhs.to_degrees();
}

template<std::floating_point Type>
inline constexpr std::strong_ordering operator<=>(const angle<Type>& lhs, const angle<Type>& rhs) noexcept {
  return lhs.to_degrees() <=> rhs.to_degrees();
}

namespace literals {

inline constexpr degrees<float32> operator""_degrees(const long double d) {
  return degrees<float32>{static_cast<float>(d)};
}

inline constexpr radians<float32> operator""_radians(const long double r) {
  return radians<float32>{static_cast<float>(r)};
}

} // namespace literals

} // namespace sbx