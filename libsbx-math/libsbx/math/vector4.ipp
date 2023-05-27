#include <libsbx/math/vector4.hpp>

#include <cmath>
#include <iomanip>
#include <string>
#include <stdexcept>

#include <libsbx/utility/hash.hpp>

#include <libsbx/core/assert.hpp>

namespace sbx::math {

template<numeric Type>
inline constexpr basic_vector4<Type>::basic_vector4() noexcept
: x{static_cast<value_type>(0)},
  y{static_cast<value_type>(0)},
  z{static_cast<value_type>(0)},
  w{static_cast<value_type>(0)} { }

template<numeric Type>
inline constexpr basic_vector4<Type>::basic_vector4(const Type value) noexcept
: x{value},
  y{value},
  z{value},
  w{value} { }

template<numeric Type>
inline constexpr basic_vector4<Type>::basic_vector4(const Type _x, const Type _y, const Type _z, const Type _w) noexcept
: x{_x},
  y{_y},
  z{_z},
  w{_w} { }

template<numeric Type>
inline constexpr basic_vector4<Type>::basic_vector4(const basic_vector3<Type>& vector, const Type _w) noexcept
: x{vector.x},
  y{vector.y},
  z{vector.z},
  w{_w} { }

template<numeric Type>
template<numeric From>
inline constexpr basic_vector4<Type>::basic_vector4(const basic_vector4<From>& other) noexcept
: x{static_cast<value_type>(other.x)},
  y{static_cast<value_type>(other.y)},
  z{static_cast<value_type>(other.z)},
  w{static_cast<value_type>(other.w)} { }

template<numeric Type>
inline constexpr auto basic_vector4<Type>::normalized(const basic_vector4& vector) noexcept -> basic_vector4<Type> {
  const auto length = vector.length();
  return length == value_type{0} ? basic_vector4<Type>{} : vector / length;
}

template<numeric Type>
template<numeric From>
inline constexpr auto basic_vector4<Type>::operator=(const basic_vector4<From>& other) noexcept -> basic_vector4<Type>& {
  x = static_cast<value_type>(other.x);
  y = static_cast<value_type>(other.y);
  z = static_cast<value_type>(other.z);
  w = static_cast<value_type>(other.w);

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator-() noexcept -> basic_vector4<Type>& {
  x = -x;
  y = -y;
  z = -z;
  w = -w;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator+=(const basic_vector4<Type>& other) noexcept -> basic_vector4<Type>& {
  x += other.x;
  y += other.y;
  z += other.z;
  w += other.w;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator-=(const basic_vector4<Type>& other) noexcept -> basic_vector4<Type>& {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  w -= other.w;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator*=(const Type scalar) noexcept -> basic_vector4<Type>& {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  w *= scalar;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator*=(const basic_vector4<Type>& other) noexcept -> basic_vector4<Type>& {
  x *= other.x;
  y *= other.y;
  z *= other.z;
  w *= other.w;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator/=(const Type scalar) -> basic_vector4<Type>& {
  if (scalar == static_cast<Type>(0)) {
    throw std::domain_error{"Division by zero"};
  }

  x /= scalar;
  y /= scalar;
  z /= scalar;
  w /= scalar;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator/=(const basic_vector4<Type>& other) -> basic_vector4<Type>& {
  if (other.x == static_cast<index_type>(0) || other.y == static_cast<Type>(0) || other.z == static_cast<Type>(0) || other.w == static_cast<Type>(0)) {
    throw std::domain_error{"Division by zero"};
  }

  x /= other.x;
  y /= other.y;
  z /= other.z;
  w /= other.w;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator[](const index_type index) -> reference {
  if (index >= static_cast<index_type>(4)) {
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
    case 2: {
      return z;
    }
    case 3: {
      return w;
    }
  }
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::operator[](const index_type index) const -> const_reference {
  if (index >= static_cast<Type>(4)) {
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
    case 2: {
      return z;
    }
    case 3: {
      return w;
    }
  }
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::length() const noexcept -> length_type {
  return std::sqrt(x * x + y * y + z * z + w * w);
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::normalize() noexcept -> void {
  const auto length = this->length();

  if (length != static_cast<length_type>(0)) {
    x /= length;
    y /= length;
    z /= length;
    w /= length;
  }
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::data() noexcept -> pointer {
  return &x;
}

template<numeric Type>
inline constexpr auto basic_vector4<Type>::data() const noexcept -> const_pointer {
  return &x;
}

template<numeric Type>
inline constexpr auto operator==(const basic_vector4<Type>& lhs, const basic_vector4<Type>& rhs) noexcept -> bool {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

template<numeric Type>
inline constexpr auto operator+(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept -> basic_vector4<Type> {
  return lhs += rhs;
}

template<numeric Type>
inline constexpr auto operator-(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept -> basic_vector4<Type> {
  return lhs -= rhs;
}

template<numeric Type>
inline constexpr auto operator*(basic_vector4<Type> lhs, const Type rhs) noexcept -> basic_vector4<Type> {
  return lhs *= rhs;
}

template<numeric Type>
inline constexpr auto operator*(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept -> basic_vector4<Type> {
  return lhs *= rhs;
}

template<numeric Type>
inline constexpr auto operator/(basic_vector4<Type> lhs, const Type rhs) -> basic_vector4<Type> {
  return lhs /= rhs;
}

template<numeric Type>
inline constexpr auto operator/(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) -> basic_vector4<Type> {
  return lhs /= rhs;
}

} // namespace sbx::math

template<sbx::math::numeric Type>
inline std::size_t std::hash<sbx::math::basic_vector4<Type>>::operator()(const sbx::math::basic_vector4<Type>& vector) const noexcept {
  auto seed = std::size_t{0};
  sbx::utility::hash_combine(seed, vector.x);
  sbx::utility::hash_combine(seed, vector.y);
  sbx::utility::hash_combine(seed, vector.z);
  sbx::utility::hash_combine(seed, vector.w);
  return seed;
}

template<sbx::math::numeric Type>
inline auto YAML::convert<sbx::math::basic_vector4<Type>>::encode(const sbx::math::basic_vector4<Type>& rhs) -> YAML::Node {
  auto node = Node{};

  node["x"] = rhs.x;
  node["y"] = rhs.y;
  node["z"] = rhs.z;
  node["w"] = rhs.w;

  return node;
}

template<sbx::math::numeric Type>
inline auto YAML::convert<sbx::math::basic_vector4<Type>>::decode(const YAML::Node& node, sbx::math::basic_vector4<Type>& rhs) -> bool {
  if (!node.IsMap()) {
    return false;
  }

  rhs.x = node["x"].as<Type>();
  rhs.y = node["y"].as<Type>();
  rhs.z = node["z"].as<Type>();
  rhs.w = node["w"].as<Type>();

  return true;
}
