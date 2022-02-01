namespace sbx {

template<typename Type>
inline constexpr basic_vector4<Type>::basic_vector4() noexcept
: x{value_type{0}},
  y{value_type{0}},
  z{value_type{0}},
  w{value_type{0}} { }

template<typename Type>
inline constexpr basic_vector4<Type>::basic_vector4(const value_type value) noexcept
: x{value},
  y{value},
  z{value},
  w{value} { }

template<typename Type>
inline constexpr basic_vector4<Type>::basic_vector4(const value_type _x, const value_type _y, const value_type _z, const value_type _w) noexcept
: x{_x},
  y{_y},
  z{_z},
  w{_w} { }

template<typename Type>
inline constexpr basic_vector4<Type>::basic_vector4(const basic_vector3<Type>& vector, const value_type _w) noexcept
: x{vector.x},
  y{vector.y},
  z{vector.z},
  w{_w} { }

template<typename Type>
template<typename From>
inline constexpr basic_vector4<Type>::basic_vector4(const basic_vector4<From>& other) noexcept
: x{static_cast<value_type>(other.x)},
  y{static_cast<value_type>(other.y)},
  z{static_cast<value_type>(other.z)},
  w{static_cast<value_type>(other.w)} {
  // Casted from type must be an arithmetic types.
  static_assert(std::is_arithmetic_v<From>, "Casted from type must be arithmetic");
}

template<typename Type>
inline constexpr basic_vector4<Type> basic_vector4<Type>::normalized(const basic_vector4<Type>& vector) noexcept {
  const auto length = vector.length();
  return length == value_type{0} ? basic_vector4<Type>{} : vector / length;
}

} // namespace sbx