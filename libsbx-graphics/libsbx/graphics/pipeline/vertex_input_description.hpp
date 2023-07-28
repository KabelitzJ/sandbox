#ifndef LIBSBX_GRAPHICS_PIPELINE_VERTEX_INPUT_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_VERTEX_INPUT_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

namespace sbx::graphics {

class vertex_input_description {

public:

  explicit vertex_input_description(std::vector<VkVertexInputBindingDescription> _binding_descriptions, std::vector<VkVertexInputAttributeDescription> _attribute_descriptions) noexcept
  : _binding_descriptions(std::move(_binding_descriptions)), 
    _attribute_descriptions(std::move(_attribute_descriptions)) {}

  auto binding_descriptions() const noexcept -> const std::vector<VkVertexInputBindingDescription>& {
    return _binding_descriptions;
  }

  auto attribute_descriptions() const noexcept -> const std::vector<VkVertexInputAttributeDescription>& {
    return _attribute_descriptions;
  }

private:

  std::vector<VkVertexInputBindingDescription> _binding_descriptions;
  std::vector<VkVertexInputAttributeDescription> _attribute_descriptions;

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
