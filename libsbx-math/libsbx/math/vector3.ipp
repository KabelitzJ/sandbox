#include <libsbx/math/vector3.hpp>

#include <stdexcept>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <libsbx/utility/assert.hpp>

#include <libsbx/utility/assert.hpp>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/vector4.hpp>

namespace sbx::math {

// template<arithmetic Type>
// inline constexpr basic_vector3<Type>::basic_vector3(const Type x, const Type y, const Type z) noexcept {
//   base_type::component(x_axis) = x;
//   base_type::component(y_axis) = y;
//   base_type::component(z_axis) = z;
// }

// template<arithmetic Type>
// inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector2<Type>& vector, const Type z) noexcept
// : base_type{vector.x(), vector.y(), z} { }

// template<arithmetic Type>
// inline constexpr basic_vector3<Type>::basic_vector3(const basic_vector4<Type>& vector) noexcept 
// : base_type{vector.x(), vector.y(), vector.z()} { }

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::absolute(const basic_vector3& vector) noexcept -> basic_vector3<Type> {
  return basic_vector3{std::abs(vector.x()), std::abs(vector.y()), std::abs(vector.z())};
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::dot(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> value_type {
  return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::cross(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> basic_vector3<Type> {
  const auto x = lhs.y() * rhs.z() - lhs.z() * rhs.y();
  const auto y = lhs.z() * rhs.x() - lhs.x() * rhs.z();
  const auto z = lhs.x() * rhs.y() - lhs.y() * rhs.x();

  return basic_vector3<Type>{x, y, z};
}

// template<arithmetic Type>
// template<std::convertible_to<Type> Scale>
// inline constexpr auto basic_vector3<Type>::lerp(const basic_vector3& lhs, const basic_vector3& rhs, const Scale& scale) noexcept -> basic_vector3<Type> {
//   return lhs + (rhs - lhs) * static_cast<value_type>(scale);
// }

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::distance_squared(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> length_type {
  return (lhs - rhs).length_squared();
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::distance(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> length_type {
  return (lhs - rhs).length();
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::x() const noexcept -> const_reference {
  return base_type::component(x_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::x() noexcept -> reference {
  return base_type::component(x_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::y() const noexcept -> const_reference {
  return base_type::component(y_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::y() noexcept -> reference {
  return base_type::component(y_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::z() const noexcept -> const_reference {
  return base_type::component(z_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::z() noexcept -> reference {
  return base_type::component(z_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector3<Type>::xy() const noexcept -> basic_vector2<value_type> {
  return basic_vector2<value_type>{x(), y()};
}

template<arithmetic Type>
auto operator<<(std::ostream& output_stream, const basic_vector3<Type>& vector) -> std::ostream& {
  return output_stream << fmt::format("({}, {}, {})", vector.x(), vector.y(), vector.z());
}

} // namespace sbx::math

template<sbx::math::arithmetic Type>
inline auto std::hash<sbx::math::basic_vector3<Type>>::operator()(const sbx::math::basic_vector3<Type>& vector) const noexcept -> std::size_t {
  auto seed = std::size_t{0};
  sbx::utility::hash_combine(seed, vector.x(), vector.y(), vector.z());
  return seed;
}

template<sbx::math::arithmetic Type>
inline auto YAML::convert<sbx::math::basic_vector3<Type>>::encode(const sbx::math::basic_vector3<Type>& rhs) -> YAML::Node {
  auto node = Node{};

  node["x"] = rhs.x();
  node["y"] = rhs.y();
  node["z"] = rhs.z();

  return node;
}

template<sbx::math::arithmetic Type>
inline auto YAML::convert<sbx::math::basic_vector3<Type>>::decode(const YAML::Node& node, sbx::math::basic_vector3<Type>& rhs) -> bool {
  if (!node.IsMap()) {
    return false;
  }

  rhs.x() = node["x"].as<Type>();
  rhs.y() = node["y"].as<Type>();
  rhs.z() = node["z"].as<Type>();

  return true;
}
