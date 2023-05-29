#ifndef LIBSBX_MATH_COLOR_HPP_
#define LIBSBX_MATH_COLOR_HPP_

#include <cmath>

#include <yaml-cpp/yaml.h>

#include <libsbx/utility/hash.hpp>

namespace sbx::math {

struct color {
  std::float_t r;
  std::float_t g;
  std::float_t b;
  std::float_t a;
}; // class color

constexpr auto operator==(const color& lhs, const color& rhs) noexcept -> bool {
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

} // namespace sbx::math

template<>
struct YAML::convert<sbx::math::color> {
  static auto encode(const sbx::math::color& color) -> Node {
    auto node = Node{};

    node["r"] = color.r;
    node["g"] = color.g;
    node["b"] = color.b;
    node["a"] = color.a;

    return node;
  }

  static auto decode(const Node& node, sbx::math::color& color) -> bool {
    if (!node.IsMap() || node.size() != 4) {
      return false;
    }

    color.r = node["r"].as<std::float_t>();
    color.g = node["g"].as<std::float_t>();
    color.b = node["b"].as<std::float_t>();
    color.a = node["a"].as<std::float_t>();

    return true;
  }
}; // struct YAML::convert

template<>
struct std::hash<sbx::math::color> {
  auto operator()(const sbx::math::color& color) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, color.r, color.g, color.b, color.a);
    return hash;
  }
}; // struct std::hash

#endif // LIBSBX_MATH_COLOR_HPP_
