#include <cmath>

namespace sbx {

template<typename Type>
inline constexpr basic_vector3<Type>::basic_vector3() noexcept
: x{}, 
  y{}, 
  z{} { }

template<typename Type>
inline constexpr basic_vector3<Type>::basic_vector3(const value_type value) noexcept
: x{value},
  y{value},
  z{value} { }

template<typename Type>
inline constexpr basic_vector3<Type>::basic_vector3(const value_type _x, const value_type _y, const value_type _z) noexcept
: x{_x},
  y{_y},
  z{_z} { }


template<typename Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator-() noexcept {
  x = -x;
  y = -y;
  z = -z;
  return *this;
}

template<typename Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator+=(const basic_vector3<Type>& other) noexcept {
  x += other.x;
  y += other.y;
  z += other.z;
  return *this;
}

template<typename Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator-=(const basic_vector3<Type>& other) noexcept {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  return *this;
}

template<typename Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator*=(const Type scalar) noexcept {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return *this;
}

template<typename Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator/=(const Type scalar) {
  if (scalar == Type{0}) {
    throw std::domain_error("Division by zero");
  }

  x /= scalar;
  y /= scalar;
  z /= scalar;
  return *this;
}

template<typename Type>
inline constexpr Type basic_vector3<Type>::length() const noexcept {
  return std::sqrt(x * x + y * y + z * z);
}

template<typename Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::normalize() noexcept {
  const auto magnitude = length();

  if (magnitude != Type{0}) {
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
  } else {
    x = Type{0};
    y = Type{0};
    z = Type{0};
  }

  return *this;
}

template<typename Type>
inline constexpr basic_vector3<Type> basic_vector3<Type>::normalized() const noexcept {
  const auto magnitude = length();

  if (magnitude != Type{0}) {
    return basic_vector3<Type>{x / magnitude, y / magnitude, z / magnitude};
  } else {
    return basic_vector3<Type>{};
  }
}

template<typename Type>
inline constexpr bool operator==(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<typename Type>
inline constexpr bool operator!=(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept {
  return !(lhs == rhs);
}

template<typename Type>
inline constexpr basic_vector3<Type> operator+(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  lhs += rhs;
  return lhs;
}

template<typename Type>
inline constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  lhs -= rhs;
  return lhs;
}

template<typename Type>
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Type rhs) noexcept {
  lhs *= rhs;
  return lhs;
}

template<typename Type>
inline constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Type rhs) {
  lhs /= rhs;
  return lhs;
}

template<typename Type>
inline constexpr std::ostream& operator<<(std::ostream& os, const basic_vector3<Type>& vector) noexcept {
  os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
  return os;
}

} // namespace sbx