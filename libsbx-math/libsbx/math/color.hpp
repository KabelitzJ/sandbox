#ifndef LIBSBX_MATH_COLOR_HPP_
#define LIBSBX_MATH_COLOR_HPP_

#include <cmath>

#include <yaml-cpp/yaml.h>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/vector4.hpp>

namespace sbx::math {

class color {

public:

  static const color black;
  static const color white;
  static const color red;
  static const color green;
  static const color blue;
  static const color transparent;

  color(std::uint32_t rgba) noexcept;

  color(std::float_t red, std::float_t green, std::float_t blue, std::float_t alpha) noexcept;

  auto r() const noexcept -> const std::float_t&;

  auto r() noexcept -> std::float_t&;

  auto g() const noexcept -> const std::float_t&;

  auto g() noexcept -> std::float_t&;

  auto b() const noexcept -> const std::float_t&;

  auto b() noexcept -> std::float_t&;

  auto a() const noexcept -> const std::float_t&;

  auto a() noexcept -> std::float_t&;

private:

  std::float_t _red;
  std::float_t _green;
  std::float_t _blue;
  std::float_t _alpha;

}; // class color

auto operator==(const color& lhs, const color& rhs) noexcept -> bool;

} // namespace sbx::math

template<>
struct YAML::convert<sbx::math::color> {
  static auto encode(const sbx::math::color& color) -> Node {
    auto node = Node{};

    node["r"] = color.r();
    node["g"] = color.g();
    node["b"] = color.b();
    node["a"] = color.a();

    return node;
  }

  static auto decode(const Node& node, sbx::math::color& color) -> bool {
    if (!node.IsMap() || node.size() != 4) {
      return false;
    }

    color.r() = node["r"].as<std::float_t>();
    color.g() = node["g"].as<std::float_t>();
    color.b() = node["b"].as<std::float_t>();
    color.a() = node["a"].as<std::float_t>();

    return true;
  }
}; // struct YAML::convert

template<>
struct std::hash<sbx::math::color> {
  auto operator()(const sbx::math::color& color) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, color.r(), color.g(), color.b(), color.a());
    return hash;
  }
}; // struct std::hash

#endif // LIBSBX_MATH_COLOR_HPP_
