#include <libsbx/math/vector2.hpp>

#include <cassert>

namespace sbx::math {

template<arithmetic Type>
inline constexpr basic_vector2<Type>::basic_vector2() noexcept
: x{value_type{0}}, 
  y{value_type{0}} { }

template<arithmetic Type>
inline constexpr basic_vector2<Type>::basic_vector2(const Type value) noexcept
: x{value}, 
  y{value} { }

template<arithmetic Type>
inline constexpr basic_vector2<Type>::basic_vector2(const Type _x, const Type _y) noexcept
: x{_x}, 
  y{_y} { }

template<arithmetic Type>
template<arithmetic Other>
inline constexpr basic_vector2<Type>::basic_vector2(const basic_vector2<Other>& other) noexcept
: x{static_cast<Type>(other.x)}, 
  y{static_cast<Type>(other.y)} { }

template<arithmetic Type>
inline constexpr basic_vector2<Type> basic_vector2<Type>::normalized(const basic_vector2& vector) noexcept requires (std::floating_point<Type>) {
  const auto length = vector.length();
  return length == static_cast<length_type>(0) ? basic_vector2<Type>{} : vector / length;
}

template<arithmetic Type>
template<arithmetic Other>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator=(const basic_vector2<Other>& other) noexcept {
  x = static_cast<Type>(other.x);
  y = static_cast<Type>(other.y);

  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator-() noexcept {
  x = -x;
  y = -y;
  
  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator+=(const basic_vector2& other) noexcept {
  x += other.x;
  y += other.y;
  
  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator-=(const basic_vector2& other) noexcept {
  x -= other.x;
  y -= other.y;
  
  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator*=(const Type scalar) noexcept {
  x *= scalar;
  y *= scalar;
  
  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type>& basic_vector2<Type>::operator/=(const Type scalar) {
  if (scalar == value_type{0}) {
    throw std::domain_error("Division by zero");
  }
  
  x /= scalar;
  y /= scalar;
  
  return *this;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type>::reference basic_vector2<Type>::operator[](const index_type index) noexcept {
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

template<arithmetic Type>
inline constexpr basic_vector2<Type>::const_reference basic_vector2<Type>::operator[](const index_type index) const noexcept {
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

template<arithmetic Type>
inline constexpr basic_vector2<Type>::length_type basic_vector2<Type>::length() const noexcept {
  return std::sqrt(x * x + y * y);
}

template<arithmetic Type>
inline constexpr void basic_vector2<Type>::normalize() noexcept requires (std::floating_point<Type>) {
  const auto length = this->length();
  
  if (length != length_type{0}) {
    x /= length;
    y /= length;
  }
}

template<arithmetic Type>
inline constexpr basic_vector2<Type>::pointer basic_vector2<Type>::data() noexcept {
  return &x;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type>::const_pointer basic_vector2<Type>::data() const noexcept {
  return &x;
}

template<arithmetic Type>
inline constexpr bool operator==(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type> operator+(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<arithmetic Type> 
inline constexpr basic_vector2<Type> operator-(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type> operator*(basic_vector2<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector2<Type> operator/(basic_vector2<Type> lhs, const Type rhs) noexcept {
  return lhs /= rhs;
}

} // namespace sbx::math
