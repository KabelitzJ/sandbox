#ifndef LIBSBX_GRAPHICS_MESH_HPP_
#define LIBSBX_GRAPHICS_MESH_HPP_

#include <filesystem>
#include <cmath>
#include <cstdint>
#include <string>

#include <yaml-cpp/yaml.h>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/pipeline/push_constant.hpp>

namespace sbx::graphics {

struct vertex {
  math::vector3 position;
  math::color color;
  math::vector3 normal;
  math::vector2 uv;
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
struct YAML::convert<sbx::graphics::vertex> {
  static auto encode(const sbx::graphics::vertex& rhs) -> YAML::Node {
    YAML::Node node;
    node["position"] = rhs.position;
    node["color"] = rhs.color;
    node["normal"] = rhs.normal;
    node["uv"] = rhs.uv;
    return node;
  }

  static auto decode(const YAML::Node& node, sbx::graphics::vertex& vertex) -> bool {
    if (!node.IsMap()) {
      return false;
    }

    vertex.position = node["position"].as<sbx::math::vector3>();
    vertex.color = node["color"].as<sbx::math::color>();
    vertex.normal = node["normal"].as<sbx::math::vector3>();
    vertex.uv = node["uv"].as<sbx::math::vector2>();

    return true;
  }
}; // struct YAML::convert<vertex>

#endif // LIBSBX_GRAPHICS_MESH_HPP_
