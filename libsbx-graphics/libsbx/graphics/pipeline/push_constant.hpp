#ifndef LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_

#include <cmath>
#include <cinttypes>

#include <yaml-cpp/yaml.h>

namespace sbx::graphics {

struct color {
  std::float_t r{};
  std::float_t g{};
  std::float_t b{};
  std::float_t a{};
}; // struct color

struct push_constant {
  alignas(16) color color{};
}; // struct push_constant

} // namespace sbx::graphics

template<>
struct YAML::convert<sbx::graphics::color> {
  static auto encode(const sbx::graphics::color& rhs) -> YAML::Node {
    YAML::Node node;
    node["r"] = rhs.r;
    node["g"] = rhs.g;
    node["b"] = rhs.b;
    node["a"] = rhs.a;
    return node;
  }

  static auto decode(const YAML::Node& node, sbx::graphics::color& color) -> bool {
    if (!node.IsMap()) {
      return false;
    }

    color.r = node["r"].as<std::float_t>();
    color.g = node["g"].as<std::float_t>();
    color.b = node["b"].as<std::float_t>();
    color.a = node["a"].as<std::float_t>();

    return true;
  }
}; // struct YAML::convert<color>

#endif // LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_
