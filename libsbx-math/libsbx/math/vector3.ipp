#include <libsbx/math/vector3.hpp>

#include <cassert>

namespace sbx::math {

template<arithmetic Type>
inline constexpr basic_vector3<Type>::basic_vector3() noexcept
: x{value_type{0}}, 
  y{value_type{0}}, 
  z{value_type{0}} { }

template<arithmetic Type>
inline constexpr basic_vector3<Type>::basic_vector3(const value_type value) noexcept
: x{value},
  y{value},
  z{value} { }

template<arithmetic Type>
inline constexpr basic_vector3<Type>::basic_vector3(const value_type _x, const value_type _y, const value_type _z) noexcept
: x{_x},
  y{_y},
  z{_z} { }

template<arithmetic Type>
inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector2<Type>& vector, const Type _z) noexcept
: x{vector.x},
  y{vector.y},
  z{_z} { }

template<arithmetic Type>
template<arithmetic Other>
inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector3<Other>& other) noexcept
: x{static_cast<value_type>(other.x)},
  y{static_cast<value_type>(other.y)},
  z{static_cast<value_type>(other.z)} { }

template<arithmetic Type>
inline constexpr basic_vector3<Type> basic_vector3<Type>::normalized(const basic_vector3& vector) noexcept {
  const auto length = vector.length();
  return length == value_type{0} ? vector : vector / length;
}

template<arithmetic Type>
inline constexpr Type basic_vector3<Type>::dot(const basic_vector3& lhs, const basic_vector3& rhs) noexcept {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> basic_vector3<Type>::cross(const basic_vector3& lhs, const basic_vector3& rhs) noexcept {
  const auto x = lhs.y * rhs.z - lhs.z * rhs.y;
  const auto y = lhs.z * rhs.x - lhs.x * rhs.z;
  const auto z = lhs.x * rhs.y - lhs.y * rhs.x;

  return basic_vector3<Type>{x, y, z};
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> basic_vector3<Type>::clamp(const basic_vector3& vector, const Type min, const Type max) noexcept {
  const auto x = std::max(min, std::min(vector.x, max));
  const auto y = std::max(min, std::min(vector.y, max));
  const auto z = std::max(min, std::min(vector.z, max));

  return basic_vector3<Type>{x, y, z};
}

template<arithmetic Type>
template<arithmetic Other>
constexpr basic_vector3<Type>& basic_vector3<Type>::operator=(const basic_vector3<Other>& other) noexcept {
  x = static_cast<value_type>(other.x);
  y = static_cast<value_type>(other.y);
  z = static_cast<value_type>(other.z);

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator-() noexcept {
  x = -x;
  y = -y;
  z = -z;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator+=(const Type scalar) noexcept {
  x += scalar;
  y += scalar;
  z += scalar;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator+=(const basic_vector3<Type>& other) noexcept {
  x += other.x;
  y += other.y;
  z += other.z;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator-=(const Type other) noexcept {
  x -= other;
  y -= other;
  z -= other;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator-=(const basic_vector3<Type>& other) noexcept {
  x -= other.x;
  y -= other.y;
  z -= other.z;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator*=(const Type scalar) noexcept {
  x *= scalar;
  y *= scalar;
  z *= scalar;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator*=(const basic_vector3<Type>& scalar) noexcept {
  x *= scalar.x;
  y *= scalar.y;
  z *= scalar.z;

  return *this;
}

template<arithmetic Type>
template<typename Other>
requires (std::is_convertible_v<Other, Type>)
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator*=(const Other& scalar) noexcept {
  x *= static_cast<value_type>(scalar);
  y *= static_cast<value_type>(scalar);
  z *= static_cast<value_type>(scalar);

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator/=(const Type scalar) {
  if (scalar == value_type{0}) {
    throw std::domain_error("Division by zero");
  }

  x /= scalar;
  y /= scalar;
  z /= scalar;

  return *this;
}

template<arithmetic Type>
template<typename Other>
requires (std::is_convertible_v<Other, Type>)
inline constexpr basic_vector3<Type>& basic_vector3<Type>::operator/=(const Other& scalar) {
  if (scalar == value_type{0}) {
    throw std::domain_error("Division by zero");
  }

  x /= static_cast<value_type>(scalar);
  y /= static_cast<value_type>(scalar);
  z /= static_cast<value_type>(scalar);

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::reference basic_vector3<Type>::operator[](const index_type index) noexcept {
  assert(index < 3);

  switch (index) {
    default:
    case 0: {
      return x;
    }
    case 1: {
      return y;
    }
    case 2: {
      return z;
    }
  }
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::const_reference basic_vector3<Type>::operator[](const index_type index) const noexcept {
  assert(index < 3);

  switch (index) {
    default:
    case 0: {
      return x;
    }
    case 1: {
      return y;
    }
    case 2: {
      return z;
    }
  }
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::length_type basic_vector3<Type>::length() const noexcept {
  return std::sqrt(x * x + y * y + z * z);
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::normalize() noexcept {
  const auto len = length();

  if (len != length_type{0}) {
    x /= len;
    y /= len;
    z /= len;
  }

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>& basic_vector3<Type>::clamp(const Type min, const Type max) noexcept {
  x = std::max(min, std::min(x, max));
  y = std::max(min, std::min(y, max));
  z = std::max(min, std::min(z, max));

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::pointer basic_vector3<Type>::data() noexcept {
  return &x;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type>::const_pointer basic_vector3<Type>::data() const noexcept {
  return &x;
}

template<arithmetic Type>
inline constexpr bool operator==(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator+(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs += rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator+(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs -= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator-(const basic_vector3<Type>& vector) noexcept {
  return -basic_vector3<Type>{vector};
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs *= rhs;
}

template<arithmetic Type, typename Other>
requires (std::is_convertible_v<Other, Type>)
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Other& rhs) noexcept {
  return lhs *= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Type rhs) {
  return lhs /= rhs;
}

template<arithmetic Type, typename Other>
requires (std::is_convertible_v<Other, Type>)
inline constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Other& rhs) {
  return lhs /= rhs;
}

} // namespace sbx::math
