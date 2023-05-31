#ifndef LIBSBX_GRAPHICS_MESH_HPP_
#define LIBSBX_GRAPHICS_MESH_HPP_

#include <filesystem>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <tiny_obj_loader.h>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/pipeline/push_constant.hpp>

namespace sbx::graphics {

struct vertex {
  math::vector3 position;
  math::vector3 normal;
  math::vector2 uv;
}; // struct vertex

constexpr auto operator==(const vertex& lhs, const vertex& rhs) noexcept -> bool {
  return lhs.position == rhs.position && lhs.normal == rhs.normal && lhs.uv == rhs.uv;
}

class mesh {

public:

  mesh(const tinyobj::attrib_t& attributes, const tinyobj::shape_t& shape);

  ~mesh() = default;

  auto vertex_buffer() const noexcept -> const buffer& {
    return *_vertex_buffer;
  }

  auto index_buffer() const noexcept -> const buffer& {
    return *_index_buffer;
  }

private:

  std::unique_ptr<buffer> _vertex_buffer{};
  std::unique_ptr<buffer> _index_buffer{};

}; // class mesh

} // namespace sbx::graphics

template<>
struct YAML::convert<sbx::graphics::vertex> {
  static auto encode(const sbx::graphics::vertex& rhs) -> YAML::Node {
    YAML::Node node;
    node["position"] = rhs.position;
    node["normal"] = rhs.normal;
    node["uv"] = rhs.uv;
    return node;
  }

  static auto decode(const YAML::Node& node, sbx::graphics::vertex& vertex) -> bool {
    if (!node.IsMap()) {
      return false;
    }

    vertex.position = node["position"].as<sbx::math::vector3>();
    vertex.normal = node["normal"].as<sbx::math::vector3>();
    vertex.uv = node["uv"].as<sbx::math::vector2>();

    return true;
  }
}; // struct YAML::convert<vertex>

template<>
struct std::hash<sbx::graphics::vertex> {
  auto operator()(const sbx::graphics::vertex& vertex) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, vertex.position, vertex.normal, vertex.uv);
    return hash;
  }
}; // struct std::hash<vertex>

#endif // LIBSBX_GRAPHICS_MESH_HPP_
