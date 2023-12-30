#include <libsbx/math/vector4.hpp>

#include <cmath>
#include <iomanip>
#include <string>
#include <stdexcept>

#include <libsbx/utility/hash.hpp>

#include <libsbx/utility/assert.hpp>

namespace sbx::math {

// template<arithmetic Type>
// inline constexpr basic_vector4<Type>::basic_vector4(const Type x, const Type y, const Type z, const Type w) noexcept { }

// template<arithmetic Type>
// inline constexpr basic_vector4<Type>::basic_vector4(const basic_vector3<Type>& vector, const Type w) noexcept
// : base_type{vector.x(), vector.y(), vector.z(), w} { }

template<arithmetic Type>
inline constexpr auto basic_vector4<Type>::x() const noexcept -> const_reference {
  return base_type::component(x_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector4<Type>::x() noexcept -> reference {
  return base_type::component(x_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector4<Type>::y() const noexcept -> const_reference {
  return base_type::component(y_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector4<Type>::y() noexcept -> reference {
  return base_type::component(y_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector4<Type>::z() const noexcept -> const_reference {
  return base_type::component(z_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector4<Type>::z() noexcept -> reference {
  return base_type::component(z_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector4<Type>::w() const noexcept -> const_reference {
  return base_type::component(w_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector4<Type>::w() noexcept -> reference {
  return base_type::component(w_axis);
}

} // namespace sbx::math

template<sbx::math::arithmetic Type>
inline std::size_t std::hash<sbx::math::basic_vector4<Type>>::operator()(const sbx::math::basic_vector4<Type>& vector) const noexcept {
  auto seed = std::size_t{0};
  sbx::utility::hash_combine(seed, vector.x());
  sbx::utility::hash_combine(seed, vector.y());
  sbx::utility::hash_combine(seed, vector.z());
  sbx::utility::hash_combine(seed, vector.w);
  return seed;
}

template<sbx::math::arithmetic Type>
inline auto YAML::convert<sbx::math::basic_vector4<Type>>::encode(const sbx::math::basic_vector4<Type>& rhs) -> YAML::Node {
  auto node = Node{};

  node["x"] = rhs.x();
  node["y"] = rhs.y();
  node["z"] = rhs.z();
  node["w"] = rhs.w;

  return node;
}

template<sbx::math::arithmetic Type>
inline auto YAML::convert<sbx::math::basic_vector4<Type>>::decode(const YAML::Node& node, sbx::math::basic_vector4<Type>& rhs) -> bool {
  if (!node.IsMap()) {
    return false;
  }

  rhs.x() = node["x"].as<Type>();
  rhs.y() = node["y"].as<Type>();
  rhs.z() = node["z"].as<Type>();
  rhs.w = node["w"].as<Type>();

  return true;
}
