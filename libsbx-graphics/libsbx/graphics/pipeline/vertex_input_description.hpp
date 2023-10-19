#ifndef LIBSBX_GRAPHICS_PIPELINE_VERTEX_INPUT_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_VERTEX_INPUT_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

namespace sbx::graphics {

struct vertex_input_description {
  std::vector<VkVertexInputBindingDescription> binding_descriptions;
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
}; // struct vertex_input

template<typename Vertex>
struct vertex_input {
  static auto description() -> vertex_input_description;
}; // struct vertex_input

template<typename Type>
concept vertex = requires {
  { vertex_input<Type>::description() } -> std::same_as<vertex_input_description>;
};

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_VERTEX_INPUT_HPP_
