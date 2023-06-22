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

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace sbx::graphics {

struct vertex3d {
  math::vector3 position;
  math::vector3 normal;
  math::vector2 uv;
}; // struct vertex

constexpr auto operator==(const vertex3d& lhs, const vertex3d& rhs) noexcept -> bool {
  return lhs.position == rhs.position && lhs.normal == rhs.normal && lhs.uv == rhs.uv;
}

template<>
struct vertex_input<vertex3d> {
  static auto description() -> vertex_input_description {
    auto binding_descriptions = std::vector<VkVertexInputBindingDescription>{};

    binding_descriptions.push_back(VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(vertex3d),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    });

    auto attribute_descriptions = std::vector<VkVertexInputAttributeDescription>{};

    attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(vertex3d, position)
    });

    attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(vertex3d, normal)
    });

    attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 2,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(vertex3d, uv)
    });

    return vertex_input_description{std::move(binding_descriptions), std::move(attribute_descriptions)};
  }
}; // struct vertex_input<vertex3d>

class mesh {

public:

  mesh(const tinyobj::attrib_t& attributes, const std::vector<tinyobj::shape_t>& shapes);

  ~mesh() = default;

  auto vertex_buffer() const noexcept -> const buffer& {
    return *_vertex_buffer;
  }

  auto index_buffer() const noexcept -> const buffer& {
    return *_index_buffer;
  }

  auto render(command_buffer& command_buffer, [[maybe_unused]] std::float_t delta_time) -> void {
    command_buffer.bind_vertex_buffer(0, *_vertex_buffer);
    command_buffer.bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    command_buffer.draw_indexed(static_cast<std::uint32_t>(_index_buffer->size() / sizeof(std::uint32_t)), 1, 0, 0, 0);
  }

private:

  std::unique_ptr<buffer> _vertex_buffer{};
  std::unique_ptr<buffer> _index_buffer{};

}; // class mesh

} // namespace sbx::graphics

template<>
struct YAML::convert<sbx::graphics::vertex3d> {
  static auto encode(const sbx::graphics::vertex3d& rhs) -> YAML::Node {
    YAML::Node node;
    node["position"] = rhs.position;
    node["normal"] = rhs.normal;
    node["uv"] = rhs.uv;
    return node;
  }

  static auto decode(const YAML::Node& node, sbx::graphics::vertex3d& vertex) -> bool {
    if (!node.IsMap()) {
      return false;
    }

    vertex.position = node["position"].as<sbx::math::vector3>();
    vertex.normal = node["normal"].as<sbx::math::vector3>();
    vertex.uv = node["uv"].as<sbx::math::vector2>();

    return true;
  }
}; // struct YAML::convert<vertex3d>

template<>
struct std::hash<sbx::graphics::vertex3d> {
  auto operator()(const sbx::graphics::vertex3d& vertex) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, vertex.position, vertex.normal, vertex.uv);
    return hash;
  }
}; // struct std::hash<vertex3d>

#endif // LIBSBX_GRAPHICS_MESH_HPP_
