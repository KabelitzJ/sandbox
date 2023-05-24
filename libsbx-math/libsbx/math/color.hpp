#ifndef LIBSBX_MATH_COLOR_HPP_
#define LIBSBX_MATH_COLOR_HPP_

#include <cmath>

#include <yaml-cpp/yaml.h>

namespace sbx::math {

struct color {
  std::float_t r;
  std::float_t g;
  std::float_t b;
  std::float_t a;
}; // class color

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

#endif // LIBSBX_MATH_COLOR_HPP_
