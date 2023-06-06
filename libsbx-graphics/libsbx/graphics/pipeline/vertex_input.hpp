#ifndef LIBSBX_GRAPHICS_PIPELINE_VERTEX_INPUT_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_VERTEX_INPUT_HPP_

namespace sbx::graphics {

template<typename Type>
struct vertex_input {
  static auto binding_description() noexcept -> std::vector<VkVertexInputBindingDescription>;
  static auto attribute_descriptions() noexcept -> std::vector<VkVertexInputAttributeDescription>;
}; // struct vertex_input

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_VERTEX_INPUT_HPP_
