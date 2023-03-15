#include <libsbx/math/vector2.hpp>

#include <cmath>
#include <iomanip>
#include <string>

#include <libsbx/core/assert.hpp>

#include <libsbx/utility/hash.hpp>

namespace sbx::math {

template<numeric Type>
inline constexpr basic_vector2<Type>::basic_vector2() noexcept
: x{value_type{0}}, 
  y{value_type{0}} { }

template<numeric Type>
inline constexpr basic_vector2<Type>::basic_vector2(const Type value) noexcept
: x{value}, 
  y{value} { }

template<numeric Type>
inline constexpr basic_vector2<Type>::basic_vector2(const Type _x, const Type _y) noexcept
: x{_x}, 
  y{_y} { }

template<numeric Type>
template<numeric Other>
inline constexpr basic_vector2<Type>::basic_vector2(const basic_vector2<Other>& other) noexcept
: x{static_cast<Type>(other.x)}, 
  y{static_cast<Type>(other.y)} { }

template<numeric Type>
inline constexpr auto basic_vector2<Type>::normalized(const basic_vector2& vector) noexcept -> basic_vector2<Type> {
  const auto length = vector.length();
  return length == static_cast<length_type>(0) ? basic_vector2<Type>{} : vector / length;
}

template<numeric Type>
template<numeric Other>
inline constexpr auto basic_vector2<Type>::operator=(const basic_vector2<Other>& other) noexcept -> basic_vector2<Type>& {
  x = static_cast<Type>(other.x);
  y = static_cast<Type>(other.y);

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::operator-() noexcept -> basic_vector2<Type>& {
  x = -x;
  y = -y;
  
  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::operator+=(const basic_vector2& other) noexcept -> basic_vector2<Type>& {
  x += other.x;
  y += other.y;
  
  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::operator-=(const basic_vector2& other) noexcept -> basic_vector2<Type>& {
  x -= other.x;
  y -= other.y;
  
  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::operator*=(const Type scalar) noexcept -> basic_vector2<Type>& {
  x *= scalar;
  y *= scalar;
  
  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::operator/=(const Type scalar) -> basic_vector2<Type>& {
  if (scalar == static_cast<Type>(0)) {
    throw std::domain_error{"Division by zero"};
  }
  
  x /= scalar;
  y /= scalar;
  
  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::operator[](const index_type index) -> reference {
  if (scalar >= static_cast<Type>(2)) {
    throw std::domain_error{"Index out of bounds"};
  }

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

template<numeric Type>
inline constexpr auto basic_vector2<Type>::operator[](const index_type index) const  -> const_reference {
  if (scalar >= static_cast<Type>(2)) {
    throw std::domain_error{"Index out of bounds"};
  }

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

template<numeric Type>
inline constexpr auto basic_vector2<Type>::length() const noexcept -> length_type {
  return std::sqrt(x * x + y * y);
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::normalize() noexcept -> void {
  const auto length = this->length();

  if (length != static_cast<length_type>(0)) {
    x /= length;
    y /= length;
  }
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::data() noexcept -> pointer {
  return &x;
}

template<numeric Type>
inline constexpr auto basic_vector2<Type>::data() const noexcept -> const_pointer {
  return &x;
}

template<numeric Type>
inline constexpr auto operator==(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept -> bool {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<numeric Type>
inline constexpr auto operator+(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept -> basic_vector2<Type> {
  return lhs += rhs;
}

template<numeric Type> 
inline constexpr auto operator-(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept -> basic_vector2<Type> {
  return lhs -= rhs;
}

template<numeric Type>
inline constexpr auto operator*(basic_vector2<Type> lhs, const Type rhs) noexcept -> basic_vector2<Type> {
  return lhs *= rhs;
}

template<numeric Type>
inline constexpr auto operator/(basic_vector2<Type> lhs, const Type rhs) -> basic_vector2<Type> {
  return lhs /= rhs;
}

} // namespace sbx::math

template<sbx::math::numeric Type>
inline auto std::hash<sbx::math::basic_vector2<Type>>::operator()(const sbx::math::basic_vector2<Type>& vector) const noexcept -> std::size_t {
  auto seed = std::size_t{0};
  sbx::utility::hash_combine(seed, vector.x, vector.y);
  return seed;
}

template<sbx::math::numeric Type>
inline auto YAML::convert<sbx::math::basic_vector2<Type>>::encode(const sbx::math::basic_vector2<Type>& rhs) -> YAML::Node {
  auto node = Node{};

  node["x"] = rhs.x;
  node["y"] = rhs.y;

  return node;
}

template<sbx::math::numeric Type>
inline auto YAML::convert<sbx::math::basic_vector2<Type>>::decode(const YAML::Node& node, sbx::math::basic_vector2<Type>& rhs) -> bool {
  if (!node.IsMap()) {
    return false;
  }

  rhs.x = node["x"].as<Type>();
  rhs.y = node["y"].as<Type>();

  return true;
}

