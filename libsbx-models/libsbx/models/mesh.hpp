#ifndef LIBSBX_MODELS_MESH_HPP_
#define LIBSBX_MODELS_MESH_HPP_

#include <filesystem>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <tiny_obj_loader.h>

#include <libsbx/utility/hash.hpp>

#include <libsbx/assets/asset.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace sbx::models {

struct vertex3d {
  math::vector3 position;
  math::vector3 normal;
  math::vector2 uv;
}; // struct vertex

constexpr auto operator==(const vertex3d& lhs, const vertex3d& rhs) noexcept -> bool {
  return lhs.position == rhs.position && lhs.normal == rhs.normal && lhs.uv == rhs.uv;
}

class mesh : public assets::asset<assets::asset_type::mesh> {

public:

  mesh(const std::filesystem::path& path);

  ~mesh() override;

  auto render(graphics::command_buffer& command_buffer) -> void {
    command_buffer.bind_vertex_buffer(0, *_vertex_buffer);
    command_buffer.bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    command_buffer.draw_indexed(static_cast<std::uint32_t>(_index_buffer->size() / sizeof(std::uint32_t)), 1, 0, 0, 0);
  }

private:

  std::unique_ptr<graphics::buffer> _vertex_buffer{};
  std::unique_ptr<graphics::buffer> _index_buffer{};

}; // class mesh

} // namespace sbx::models

template<>
struct sbx::graphics::vertex_input<sbx::models::vertex3d> {
  static auto description() -> sbx::graphics::vertex_input_description {
    auto binding_descriptions = std::vector<VkVertexInputBindingDescription>{};

    binding_descriptions.push_back(VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(sbx::models::vertex3d),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    });

    auto attribute_descriptions = std::vector<VkVertexInputAttributeDescription>{};

    attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(sbx::models::vertex3d, position)
    });

    attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(sbx::models::vertex3d, normal)
    });

    attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 2,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(sbx::models::vertex3d, uv)
    });

    return sbx::graphics::vertex_input_description{std::move(binding_descriptions), std::move(attribute_descriptions)};
  }
}; // struct sbx::graphics::vertex_input<sbx::models::vertex3d>

template<>
struct YAML::convert<sbx::models::vertex3d> {
  static auto encode(const sbx::models::vertex3d& rhs) -> YAML::Node {
    YAML::Node node;
    node["position"] = rhs.position;
    node["normal"] = rhs.normal;
    node["uv"] = rhs.uv;
    return node;
  }

  static auto decode(const YAML::Node& node, sbx::models::vertex3d& vertex) -> bool {
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
struct std::hash<sbx::models::vertex3d> {
  auto operator()(const sbx::models::vertex3d& vertex) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, vertex.position, vertex.normal, vertex.uv);
    return hash;
  }
}; // struct std::hash<vertex3d>

#endif // LIBSBX_MODELS_MESH_HPP_
