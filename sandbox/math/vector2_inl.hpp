#include <cassert>
#include <cmath>
#include <string>

namespace sbx {

template<typename Type>
inline constexpr basic_vector2<Type>::basic_vector2() noexcept
: x{value_type{0}}, 
  y{value_type{0}} { }

template<typename Type>
inline constexpr basic_vector2<Type>::basic_vector2(const Type value) noexcept
: x{value}, 
  y{value} { }

template<typename Type>
inline constexpr basic_vector2<Type>::basic_vector2(const Type _x, const Type _y) noexcept
: x{_x}, 
  y{_y} { }

template<typename Type>
template<typename From>
inline constexpr basic_vector2<Type>::basic_vector2(const basic_vector2<From>& other) noexcept
: x{static_cast<Type>(other.x)}, 
  y{static_cast<Type>(other.y)} {
  // Casted from type must be an arithmetic types.
  static_assert(std::is_arithmetic_v<From>, "Casted from type must be arithmetic");
}

template<typename Type>
constexpr basic_vector2<Type> basic_vector2<Type>::normalized(const basic_vector2<Type>& vector) noexcept {
  const auto length = vector.length();
  return length == value_type{0} ? basic_vector2<Type>{} : vector / length;
}

template<typename Type>
template<typename From>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator=(const basic_vector2<From>& other) noexcept {
  // Casted from type must be an arithmetic types.
  static_assert(std::is_arithmetic_v<From>, "Casted from type must be arithmetic");

  if (*this != other) {
    x = static_cast<Type>(other.x);
    y = static_cast<Type>(other.y);
  }
  
  return *this;
}

template<typename Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator-() noexcept {
  x = -x;
  y = -y;
  
  return *this;
}

template<typename Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator+=(const basic_vector2<Type>& other) noexcept {
  x += other.x;
  y += other.y;
  
  return *this;
}

template<typename Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator-=(const basic_vector2<Type>& other) noexcept {
  x -= other.x;
  y -= other.y;
  
  return *this;
}

template<typename Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator*=(const Type scalar) noexcept {
  x *= scalar;
  y *= scalar;
  
  return *this;
}

template<typename Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator/=(const Type scalar) {
  if (scalar == value_type{0}) {
    throw std::domain_error("Division by zero");
  }
  
  x /= scalar;
  y /= scalar;
  
  return *this;
}

template<typename Type>
inline constexpr typename basic_vector2<Type>::reference basic_vector2<Type>::operator[](const index_type index) noexcept {
  assert(index < 2);

  switch (index) {
    default:
    case 0: {
      return x;
    }
    case 1: {
      return y;
    }
  }
}

template<typename Type>
inline constexpr typename basic_vector2<Type>::const_reference basic_vector2<Type>::operator[](const index_type index) const noexcept {
  assert(index < 2);

  switch (index) {
    default:
    case 0: {
      return x;
    }
    case 1: {
      return y;
    }
  }
}

template<typename Type>
inline constexpr typename basic_vector2<Type>::length_type basic_vector2<Type>::length() const noexcept {
  return std::sqrt(x * x + y * y);
}

template<typename Type>
inline constexpr void basic_vector2<Type>::normalize() noexcept {
  const auto length = this->length();
  
  if (length != length_type{0}) {
    x /= length;
    y /= length;
  }
}

template<typename Type>
inline constexpr typename basic_vector2<Type>::pointer basic_vector2<Type>::data() noexcept {
  return &x;
}

template<typename Type>
inline constexpr typename basic_vector2<Type>::const_pointer basic_vector2<Type>::data() const noexcept {
  return &x;
}

template<typename Type>
inline constexpr bool operator==(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<typename Type>
inline constexpr bool operator!=(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept {
  return !(lhs == rhs);
}

template<typename Type>
inline constexpr basic_vector2<Type> operator+(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<typename Type> 
inline constexpr basic_vector2<Type> operator-(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<typename Type>
inline constexpr basic_vector2<Type> operator*(basic_vector2<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<typename Type>
inline constexpr basic_vector2<Type> operator/(basic_vector2<Type> lhs, const Type rhs) noexcept {
  return lhs /= rhs;
}

template<typename Type>
inline constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_vector2<Type>& vector) {
  return output_stream << "(" << vector.x << ", " << vector.y << ")";
}

template<typename Type>
inline constexpr std::ofstream& operator<<(std::ofstream& output_stream, const basic_vector2<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return output_stream;
}

template<typename OutputStream, typename Type>
inline constexpr OutputStream& operator<<(OutputStream& output_stream, const basic_vector2<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return output_stream;
}

template<typename Type>
inline constexpr std::istream& operator>>(std::istream& input_stream, basic_vector2<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return input_stream;
}

template<typename InputStream, typename Type>
inline constexpr InputStream& operator>>(InputStream& input_stream, basic_vector2<Type>& vector) {
  // [TODO] KAJ 2022-01-31 09:48 - Find a suitable format for vectors and implement a parser for that format.
  return input_stream;
}

} // namespace sbx