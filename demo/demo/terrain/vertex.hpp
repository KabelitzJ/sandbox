#ifndef DEMO_TERRAIN_VERTEX_HPP_
#define DEMO_TERRAIN_VERTEX_HPP_

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace demo {

struct vertex {
  sbx::math::vector2 position;
  std::uint32_t height_index;
  sbx::math::vector3 normal;
  sbx::math::vector2 uv;
}; // struct vertex

auto operator==(const vertex& lhs, const vertex& rhs) noexcept -> bool;

} // namespace demo

template<>
struct sbx::graphics::vertex_input<demo::vertex> {

  static auto description() -> sbx::graphics::vertex_input_description {
    auto result = sbx::graphics::vertex_input_description{};

    result.binding_descriptions.push_back(VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(demo::vertex),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(demo::vertex, position)
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32_UINT,
      .offset = offsetof(demo::vertex, height_index)
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 2,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(demo::vertex, normal)
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 3,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(demo::vertex, uv)
    });

    return result;
  }

}; // struct sbx::graphics::vertex_input

template<>
struct std::hash<demo::vertex> {

  auto operator()(const demo::vertex& vertex) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, vertex.position, vertex.height_index, vertex.normal, vertex.uv);
    return hash;
  }

}; // struct std::hash<demo::vertex>

#endif // DEMO_TERRAIN_VERTEX_HPP_
