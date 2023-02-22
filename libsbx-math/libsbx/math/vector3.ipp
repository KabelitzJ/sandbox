#include <libsbx/math/vector3.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <libsbx/core/assert.hpp>

#include <libsbx/utility/hash.hpp>

namespace sbx::math {

template<numeric Type>
inline constexpr basic_vector3<Type>::basic_vector3() noexcept
: x{static_cast<value_type>(0)}, 
  y{static_cast<value_type>(0)}, 
  z{static_cast<value_type>(0)} { }

template<numeric Type>
inline constexpr basic_vector3<Type>::basic_vector3(const value_type value) noexcept
: x{value},
  y{value},
  z{value} { }

template<numeric Type>
inline constexpr basic_vector3<Type>::basic_vector3(const value_type _x, const value_type _y, const value_type _z) noexcept
: x{_x},
  y{_y},
  z{_z} { }

// TODO: Needs vector2 first
// template<numeric Type>
// inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector2<Type>& vector, const Type _z) noexcept
// : x{vector.x},
//   y{vector.y},
//   z{_z} { }

template<numeric Type>
template<numeric Other>
inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector3<Other>& other) noexcept
: x{static_cast<value_type>(other.x)},
  y{static_cast<value_type>(other.y)},
  z{static_cast<value_type>(other.z)} { }

template<numeric Type>
inline constexpr auto basic_vector3<Type>::normalized(const basic_vector3& vector) noexcept -> basic_vector3<Type> {
  const auto length = vector.length();
  return length == static_cast<value_type>(0) ? vector : vector / length;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::dot(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> value_type {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::cross(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> basic_vector3<Type> {
  const auto x = lhs.y * rhs.z - lhs.z * rhs.y;
  const auto y = lhs.z * rhs.x - lhs.x * rhs.z;
  const auto z = lhs.x * rhs.y - lhs.y * rhs.x;

  return basic_vector3<Type>{x, y, z};
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::clamp(const basic_vector3& vector, const Type min, const Type max) noexcept -> basic_vector3<Type> {
  const auto x = std::max(min, std::min(vector.x, max));
  const auto y = std::max(min, std::min(vector.y, max));
  const auto z = std::max(min, std::min(vector.z, max));

  return basic_vector3<Type>{x, y, z};
}

template<numeric Type>
template<numeric Other>
constexpr auto basic_vector3<Type>::operator=(const basic_vector3<Other>& other) noexcept -> basic_vector3& {
  x = static_cast<value_type>(other.x);
  y = static_cast<value_type>(other.y);
  z = static_cast<value_type>(other.z);

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator-() noexcept -> basic_vector3& {
  x = -x;
  y = -y;
  z = -z;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator+=(const Type scalar) noexcept -> basic_vector3& {
  x += scalar;
  y += scalar;
  z += scalar;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator+=(const basic_vector3& other) noexcept -> basic_vector3& {
  x += other.x;
  y += other.y;
  z += other.z;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator-=(const Type other) noexcept -> basic_vector3& {
  x -= other;
  y -= other;
  z -= other;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator-=(const basic_vector3& other) noexcept -> basic_vector3& {
  x -= other.x;
  y -= other.y;
  z -= other.z;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator*=(const Type scalar) noexcept -> basic_vector3& {
  x *= scalar;
  y *= scalar;
  z *= scalar;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator*=(const basic_vector3& scalar) noexcept -> basic_vector3& {
  x *= scalar.x;
  y *= scalar.y;
  z *= scalar.z;

  return *this;
}

template<numeric Type>
template<numeric Other>
inline constexpr auto basic_vector3<Type>::operator*=(const Other& scalar) noexcept -> basic_vector3& {
  x *= static_cast<value_type>(scalar);
  y *= static_cast<value_type>(scalar);
  z *= static_cast<value_type>(scalar);

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator/=(const Type scalar) noexcept -> basic_vector3& {
  core::assert_that(scalar != static_cast<value_type>(0), "Division by zero");

  x /= scalar;
  y /= scalar;
  z /= scalar;

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator/=(const basic_vector3& scalar) noexcept -> basic_vector3& {
  core::assert_that(scalar.x != static_cast<value_type>(0), "Division by zero");

  x /= scalar.x;
  y /= scalar.y;
  z /= scalar.z;

  return *this;
}

template<numeric Type>
template<numeric Other>
inline constexpr auto basic_vector3<Type>::operator/=(const Other& scalar) noexcept -> basic_vector3& {
  core::assert_that(scalar != static_cast<value_type>(0), "Division by zero");

  x /= static_cast<value_type>(scalar);
  y /= static_cast<value_type>(scalar);
  z /= static_cast<value_type>(scalar);

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator[](const index_type index) noexcept -> basic_vector3& {
  core::assert_that(index < 3, "Invalid index");

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

template<numeric Type>
inline constexpr auto basic_vector3<Type>::operator[](const index_type index) const noexcept -> const basic_vector3& {
  core::assert_that(index < 3, "Invalid index");

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

template<numeric Type>
inline constexpr auto basic_vector3<Type>::length() const noexcept -> length_type {
  return std::sqrt(x * x + y * y + z * z);
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::length_squared() const noexcept -> length_type {
  return x * x + y * y + z * z;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::normalize() noexcept -> basic_vector3& {
  const auto len = length();

  if (len != length_type{0}) {
    x /= len;
    y /= len;
    z /= len;
  }

  return *this;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::clamp(const Type min, const Type max) noexcept -> basic_vector3& {
  x = std::max(min, std::min(x, max));
  y = std::max(min, std::min(y, max));
  z = std::max(min, std::min(z, max));

  return *this;
}

template<numeric Type>
template<std::floating_point Scale>
inline constexpr auto basic_vector3<Type>::lerp(const basic_vector3& lhs, const basic_vector3& rhs, const Scale scale) noexcept -> basic_vector3 {
  core::assert_that(scale >= static_cast<Scale>(0) && scale <= static_cast<Scale>(1), "Invalid scale");
  return lhs + (rhs - lhs) * scale;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::data() noexcept -> pointer {
  return &x;
}

template<numeric Type>
inline constexpr auto basic_vector3<Type>::data() const noexcept -> const_pointer {
  return &x;
}

template<numeric Type>
inline constexpr bool operator==(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator+(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs += rhs;
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator+(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs -= rhs;
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator-(const basic_vector3<Type>& vector) noexcept {
  return -basic_vector3<Type>{vector};
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs *= rhs;
}

template<numeric Type, numeric Other>
inline constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Other& rhs) noexcept {
  return lhs *= rhs;
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Type rhs) noexcept {
  return lhs /= rhs;
}

template<numeric Type>
inline constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
  return lhs /= rhs;
}

template<numeric Type, numeric Other>
inline constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Other rhs) noexcept {
  return lhs /= rhs;
}

template<numeric Type>
auto operator<<(std::ostream& output_stream, const basic_vector3<Type>& vector) -> std::ostream& {
  return output_stream << fmt::format("({}, {}, {})", vector.x, vector.y, vector.z);
}

template<numeric Type>
auto operator<<(io::node& node, const basic_vector3<Type>& vector) -> io::node& {
  node["x"] = vector.x;
  node["y"] = vector.y;
  node["z"] = vector.y;

  return node;
}

} // namespace sbx::math

template<sbx::math::numeric Type>
inline std::size_t std::hash<sbx::math::basic_vector3<Type>>::operator()(const sbx::math::basic_vector3<Type>& vector) const noexcept {
  auto seed = std::size_t{0};
  sbx::utility::hash_combine(seed, vector.x, vector.y, vector.z);
  return seed;
}
