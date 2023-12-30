#include <libsbx/math/vector2.hpp>

#include <cmath>
#include <iomanip>
#include <string>

#include <libsbx/utility/assert.hpp>

#include <libsbx/utility/hash.hpp>

namespace sbx::math {

// template<arithmetic Type>
// inline constexpr basic_vector2<Type>::basic_vector2(const Type x, const Type y) noexcept
// : base_type{x, y} { }

template<arithmetic Type>
inline constexpr auto basic_vector2<Type>::x() const noexcept -> const_reference {
  return base_type::component(x_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector2<Type>::x() noexcept -> reference {
  return base_type::component(x_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector2<Type>::y() const noexcept -> const_reference {
  return base_type::component(y_axis);
}

template<arithmetic Type>
inline constexpr auto basic_vector2<Type>::y() noexcept -> reference {
  return base_type::component(y_axis);
}

template<arithmetic Type>
inline constexpr auto operator==(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept -> bool {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

template<arithmetic Type>
inline constexpr auto operator+(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept -> basic_vector2<Type> {
  return lhs += rhs;
}

template<arithmetic Type, arithmetic Other>
inline constexpr auto operator+(basic_vector2<Type> lhs, const basic_vector2<Other>& rhs) noexcept -> basic_vector2<Type> {
  return lhs += rhs;
}

template<arithmetic Type> 
inline constexpr auto operator-(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept -> basic_vector2<Type> {
  return lhs -= rhs;
}

template<arithmetic Type>
inline constexpr auto operator*(basic_vector2<Type> lhs, const Type rhs) noexcept -> basic_vector2<Type> {
  return lhs *= rhs;
}

template<arithmetic Type, arithmetic Other>
inline constexpr auto operator*(basic_vector2<Type> lhs, const basic_vector2<Other>& rhs) noexcept -> basic_vector2<Type> {
  return lhs *= rhs;
}

template<arithmetic Type>
inline constexpr auto operator/(basic_vector2<Type> lhs, const Type rhs) -> basic_vector2<Type> {
  return lhs /= rhs;
}

} // namespace sbx::math

template<sbx::math::arithmetic Type>
inline auto std::hash<sbx::math::basic_vector2<Type>>::operator()(const sbx::math::basic_vector2<Type>& vector) const noexcept -> std::size_t {
  auto seed = std::size_t{0};
  sbx::utility::hash_combine(seed, vector.x(), vector.y());
  return seed;
}

template<sbx::math::arithmetic Type>
inline auto YAML::convert<sbx::math::basic_vector2<Type>>::encode(const sbx::math::basic_vector2<Type>& rhs) -> YAML::Node {
  auto node = Node{};

  node["x"] = rhs.x();
  node["y"] = rhs.y();

  return node;
}

template<sbx::math::arithmetic Type>
inline auto YAML::convert<sbx::math::basic_vector2<Type>>::decode(const YAML::Node& node, sbx::math::basic_vector2<Type>& rhs) -> bool {
  if (!node.IsMap()) {
    return false;
  }

  rhs.x() = node["x"].as<Type>();
  rhs.y() = node["y"].as<Type>();

  return true;
}

template<sbx::math::arithmetic Type>
template<typename ParseContext>
inline constexpr auto fmt::formatter<sbx::math::basic_vector2<Type>>::parse(ParseContext& context) -> decltype(context.begin()) {
  return context.begin();
}

template<sbx::math::arithmetic Type>
template<typename FormatContext>
inline auto fmt::formatter<sbx::math::basic_vector2<Type>>::format(const sbx::math::basic_vector2<Type>& vector, FormatContext& context) -> decltype(context.out()) {
  return fmt::format_to(context.out(), "({},{})", vector.x(), vector.y());
}

