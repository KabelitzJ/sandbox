#ifndef LIBSBX_GRAPHICS_MESH_HPP_
#define LIBSBX_GRAPHICS_MESH_HPP_

#include <filesystem>
#include <cmath>
#include <cstdint>
#include <string>

#include <yaml-cpp/yaml.h>

#include <libsbx/graphics/buffer/buffer.hpp>

namespace sbx::graphics {

struct vector2 {
  std::float_t x;
  std::float_t y;
}; // struct vector2

struct color {
  std::float_t r;
  std::float_t g;
  std::float_t b;
  std::float_t a;
}; // struct color

struct vertex {
  vector2 position;
  color color;
}; // struct vertex

class mesh {

public:

  mesh(const std::filesystem::path& path);

  ~mesh() = default;

  auto name() const noexcept -> const std::string& {
    return _name;
  }

  auto version() const noexcept -> const std::string& {
    return _version;
  }

  auto vertex_buffer() const noexcept -> const buffer& {
    return *_vertex_buffer;
  }

  auto index_buffer() const noexcept -> const buffer& {
    return *_index_buffer;
  }

private:

  std::string _name{};
  std::string _version{};
  std::unique_ptr<buffer> _vertex_buffer{};
  std::unique_ptr<buffer> _index_buffer{};

}; // class mesh

} // namespace sbx::graphics

template<>
struct YAML::convert<sbx::graphics::vector2> {
  static auto encode(const sbx::graphics::vector2& rhs) -> YAML::Node {
    YAML::Node node;
    node["x"] = rhs.x;
    node["y"] = rhs.y;
    return node;
  }

  static auto decode(const YAML::Node& node, sbx::graphics::vector2& vector) -> bool {
    if (!node.IsMap()) {
      return false;
    }

    vector.x = node["x"].as<std::float_t>();
    vector.y = node["y"].as<std::float_t>();

    return true;
  }
}; // struct YAML::convert<vector2>

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

template<>
struct YAML::convert<sbx::graphics::vertex> {
  static auto encode(const sbx::graphics::vertex& rhs) -> YAML::Node {
    YAML::Node node;
    node["position"] = rhs.position;
    node["color"] = rhs.color;
    return node;
  }

  static auto decode(const YAML::Node& node, sbx::graphics::vertex& vertex) -> bool {
    if (!node.IsMap()) {
      return false;
    }

    vertex.position = node["position"].as<sbx::graphics::vector2>();
    vertex.color = node["color"].as<sbx::graphics::color>();

    return true;
  }
}; // struct YAML::convert<vertex>

#endif // LIBSBX_GRAPHICS_MESH_HPP_
