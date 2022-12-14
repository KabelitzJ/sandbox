#include <libsbx/math/vector4.hpp>

#include <cassert>

namespace sbx::math {

template<arithmetic Type>
inline constexpr basic_vector4<Type>::basic_vector4() noexcept
: x{value_type{0}},
  y{value_type{0}},
  z{value_type{0}},
  w{value_type{0}} { }

template<arithmetic Type>
inline constexpr basic_vector4<Type>::basic_vector4(const Type value) noexcept
: x{value},
  y{value},
  z{value},
  w{value} { }

template<arithmetic Type>
inline constexpr basic_vector4<Type>::basic_vector4(const Type _x, const Type _y, const Type _z, const Type _w) noexcept
: x{_x},
  y{_y},
  z{_z},
  w{_w} { }

template<arithmetic Type>
inline constexpr basic_vector4<Type>::basic_vector4(const basic_vector3<Type>& vector, const Type _w) noexcept
: x{vector.x},
  y{vector.y},
  z{vector.z},
  w{_w} { }

template<arithmetic Type>
template<arithmetic From>
inline constexpr basic_vector4<Type>::basic_vector4(const basic_vector4<From>& other) noexcept
: x{static_cast<value_type>(other.x)},
  y{static_cast<value_type>(other.y)},
  z{static_cast<value_type>(other.z)},
  w{static_cast<value_type>(other.w)} { }

template<arithmetic Type>
inline constexpr basic_vector4<Type> basic_vector4<Type>::normalized(const basic_vector4& vector) noexcept {
  const auto length = vector.length();
  return length == value_type{0} ? basic_vector4<Type>{} : vector / length;
}

template<arithmetic Type>
template<arithmetic From>
inline constexpr basic_vector4<Type>& basic_vector4<Type>::operator=(const basic_vector4<From>& other) noexcept {
  x = static_cast<value_type>(other.x);
  y = static_cast<value_type>(other.y);
  z = static_cast<value_type>(other.z);
  w = static_cast<value_type>(other.w);

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>& basic_vector4<Type>::operator-() noexcept {
  x = -x;
  y = -y;
  z = -z;
  w = -w;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>& basic_vector4<Type>::operator+=(const basic_vector4<Type>& other) noexcept {
  x += other.x;
  y += other.y;
  z += other.z;
  w += other.w;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>& basic_vector4<Type>::operator-=(const basic_vector4<Type>& other) noexcept {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  w -= other.w;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>& basic_vector4<Type>::operator*=(const Type scalar) noexcept {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  w *= scalar;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>& basic_vector4<Type>::operator*=(const basic_vector4<Type>& other) noexcept {
  x *= other.x;
  y *= other.y;
  z *= other.z;
  w *= other.w;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>& basic_vector4<Type>::operator/=(const Type scalar) {
  if (scalar == value_type{0}) {
    throw std::domain_error("Division by zero");
  }

  x /= scalar;
  y /= scalar;
  z /= scalar;
  w /= scalar;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>& basic_vector4<Type>::operator/=(const basic_vector4<Type>& other) {
  if (other.x == value_type{0} || other.y == value_type{0} || other.z == value_type{0} || other.w == value_type{0}) {
    throw std::domain_error("Division by zero");
  }

  x /= other.x;
  y /= other.y;
  z /= other.z;
  w /= other.w;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>::reference basic_vector4<Type>::operator[](const index_type index) noexcept {
  assert(index < 4);

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
    case 3: {
      return w;
    }
  }
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>::const_reference basic_vector4<Type>::operator[](const index_type index) const noexcept {
  assert(index < 4);

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
    case 3: {
      return w;
    }
  }
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>::length_type basic_vector4<Type>::length() const noexcept {
  return std::sqrt(x * x + y * y + z * z + w * w);
}

template<arithmetic Type>
inline constexpr void basic_vector4<Type>::normalize() noexcept {
  const auto length = this->length();

  if (length != length_type{0}) {
    x /= length;
    y /= length;
    z /= length;
    w /= length;
  }
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>::pointer basic_vector4<Type>::data() noexcept {
  return &x;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type>::const_pointer basic_vector4<Type>::data() const noexcept {
  return &x;
}

template<arithmetic Type>
inline constexpr bool operator==(const basic_vector4<Type>& lhs, const basic_vector4<Type>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

template<arithmetic Type>
inline constexpr bool operator!=(const basic_vector4<Type>& lhs, const basic_vector4<Type>& rhs) noexcept {
  return !(lhs == rhs);
}

template<arithmetic Type>
inline constexpr basic_vector4<Type> operator+(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type> operator-(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type> operator*(basic_vector4<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type> operator*(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept {
  return lhs *= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type> operator/(basic_vector4<Type> lhs, const Type rhs) {
  return lhs /= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type> operator/(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) {
  return lhs /= rhs;
}

} // namespace sbx::math
