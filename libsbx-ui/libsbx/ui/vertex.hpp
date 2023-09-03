#ifndef LIBSBX_UI_VERTEX_HPP_
#define LIBSBX_UI_VERTEX_HPP_

#include <libsbx/math/vector2.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace sbx::ui {

struct vertex {
  math::vector2 position;
}; // struct vertex

constexpr auto operator==(const vertex& lhs, const vertex& rhs) noexcept -> bool {
  return lhs.position == rhs.position;
}

} // namespace sbx::ui

template<>
struct sbx::graphics::vertex_input<sbx::ui::vertex> {
  static auto description() -> sbx::graphics::vertex_input_description {
    auto binding_descriptions = std::vector<VkVertexInputBindingDescription>{};

    binding_descriptions.push_back(VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(sbx::ui::vertex),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    });

    auto attribute_descriptions = std::vector<VkVertexInputAttributeDescription>{};

    attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(sbx::ui::vertex, position)
    });

    return sbx::graphics::vertex_input_description{std::move(binding_descriptions), std::move(attribute_descriptions)};
  }
}; // struct sbx::graphics::vertex_input

#endif // LIBSBX_UI_VERTEX_HPP_
