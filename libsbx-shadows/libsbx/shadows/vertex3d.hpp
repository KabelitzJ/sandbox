#ifndef LIBSBX_SHADOWS_VERTEX3D_HPP_
#define LIBSBX_SHADOWS_VERTEX3D_HPP_

#include <libsbx/math/vector3.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::shadows {

struct vertex3d {
  math::vector3 position;
}; // struct vertex3d

constexpr auto operator==(const vertex3d& lhs, const vertex3d& rhs) noexcept -> bool {
  return lhs.position == rhs.position;
}

} // namespace sbx::shadows

template<>
struct sbx::graphics::vertex_input<sbx::shadows::vertex3d> {
  static auto description() -> sbx::graphics::vertex_input_description {
    auto binding_descriptions = std::vector<VkVertexInputBindingDescription>{};

    // [NOTE] KAJ 2023-10-24 : We use sbx::models::vertex3d as the base for sbx::shadows::vertex3d, so we can use the same binding description.
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
      .offset = offsetof(sbx::shadows::vertex3d, position)
    });

    return sbx::graphics::vertex_input_description{std::move(binding_descriptions), std::move(attribute_descriptions)};
  }
}; // struct sbx::graphics::vertex_input<sbx::shadows::vertex3d>

template<>
struct std::hash<sbx::shadows::vertex3d> {
  auto operator()(const sbx::shadows::vertex3d& vertex) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, vertex.position);
    return hash;
  }
}; // struct std::hash<sbx::shadows::vertex3d>

#endif // LIBSBX_SHADOWS_VERTEX3D_HPP_
