#ifndef LIBSBX_MODELS_VERTEX3D_HPP_
#define LIBSBX_MODELS_VERTEX3D_HPP_

#include <utility>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

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

} // namespace sbx::models

template<>
struct sbx::graphics::vertex_input<sbx::models::vertex3d> {
  static auto description() -> sbx::graphics::vertex_input_description {
    auto result = vertex_input_description{};

    result.binding_descriptions.push_back(VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(sbx::models::vertex3d),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(sbx::models::vertex3d, position)
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(sbx::models::vertex3d, normal)
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 2,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(sbx::models::vertex3d, uv)
    });

    return result;
  }
}; // struct sbx::graphics::vertex_input<sbx::models::vertex3d>

template<>
struct std::hash<sbx::models::vertex3d> {
  auto operator()(const sbx::models::vertex3d& vertex) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, vertex.position, vertex.normal, vertex.uv);
    return hash;
  }
}; // struct std::hash<vertex3d>

#endif // LIBSBX_MODELS_VERTEX3D_HPP_
