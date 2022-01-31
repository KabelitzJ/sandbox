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
: x{x},
  y{y},
  z{z},
  w{w} { }

template<typename Type>
inline constexpr basic_vector4<Type>::basic_vector4(const basic_vector3<Type>& vector, const value_type _w) noexcept
: x{vector.x},
  y{vector.y},
  z{vector.z},
  w{w} { }

} // namespace sbx