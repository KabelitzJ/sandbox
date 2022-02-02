#include <cassert>
#include <cmath>
#include <string>

namespace sbx {

template<typename Type>
inline constexpr basic_vector3<Type>::basic_vector3() noexcept
: x{value_type{0}}, 
  y{value_type{0}}, 
  z{value_type{0}} { }

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
inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector2<Type>& vector, const Type _z) noexcept
: x{vector.x},
  y{vector.y},
  z{_z} { }

template<typename Type>
template<typename From>
inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector3<From>& other) noexcept
: x{static_cast<value_type>(other.x)},
  y{static_cast<value_type>(other.y)},
  z{static_cast<value_type>(other.z)} {
  // Casted from type must be an arithmetic types.
  static_assert(std::is_arithmetic_v<From>, "Casted from type must be arithmetic");
}

template<typename Type>
inline constexpr basic_vector3<Type> basic_vector3<Type>::normalized(const basic_vector3<value_type>& vector) noexcept {
  const auto length = vector.length();
  return length == value_type{0} ? basic_vector3<Type>{} : vector / length;
}

template<typename Type>
inline constexpr Type basic_vector3<Type>::dot(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<typename Type>
inline constexpr basic_vector3<Type> basic_vector3<Type>::cross(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept {
  constexpr auto x = lhs.y * rhs.z - lhs.z * rhs.y;
  constexpr auto y = lhs.z * rhs.x - lhs.x * rhs.z;
  constexpr auto z = lhs.x * rhs.y - lhs.y * rhs.x;

  return basic_vector3<Type>{x, y, z};
}

template<typename Type>
template<typename From>
constexpr basic_vector3<Type>& basic_vector3<Type>::operator=(const basic_vector3<From>& other) noexcept {
  // Casted from type must be an arithmetic types.
  static_assert(std::is_arithmetic_v<From>, "Casted from type must be arithmetic");

  if (*this != other) {
    x = static_cast<value_type>(other.x);
    y = static_cast<value_type>(other.y);
    z = static_cast<value_type>(other.z);
  }

  return *this;
}

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
  if (scalar == value_type{0}) {
    throw std::domain_error("Division by zero");
  }

  x /= scalar;
  y /= scalar;
  z /= scalar;

  return *this;
}

template<typename Type>
inline constexpr typename basic_vector3<Type>::reference basic_vector3<Type>::operator[](const index_type index) noexcept {
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

template<typename Type>
inline constexpr typename basic_vector3<Type>::const_reference basic_vector3<Type>::operator[](const index_type index) const noexcept {
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

template<typename Type>
inline constexpr typename basic_vector3<Type>::length_type basic_vector3<Type>::length() const noexcept {
  return std::sqrt(x * x + y * y + z * z);
}

template<typename Type>
inline constexpr void basic_vector3<Type>::normalize() noexcept {
  const auto length = this->length();

  if (length != length_type{0}) {
    x /= length;
    y /= length;
    z /= length;
  }
}

template<typename Type>
inline constexpr typename basic_vector3<Type>::pointer basic_vector3<Type>::data() noexcept {
  return &x;
}

template<typename Type>
inline constexpr typename basic_vector3<Type>::const_pointer basic_vector3<Type>::data() const noexcept {
  return &x;
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
  return lhs += rhs;
}

template<typename Type>
inline constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<typename Type>
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<typename Type>
inline constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Type rhs) {
  return lhs /= rhs;
}

template<typename Type>
inline constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_vector3<Type>& vector) {
  return output_stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
}

template<typename Type>
inline constexpr std::ofstream& operator<<(std::ofstream& output_stream, const basic_vector3<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return output_stream;
}

template<typename OutputStream, typename Type>
inline constexpr OutputStream& operator<<(OutputStream& output_stream, const basic_vector3<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return output_stream;
}

template<typename Type>
inline constexpr std::istream& operator>>(std::istream& input_stream, basic_vector3<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return input_stream;
}

template<typename InputStream, typename Type>
inline constexpr InputStream& operator>>(InputStream& input_stream, basic_vector3<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return input_stream;
}

} // namespace sbx