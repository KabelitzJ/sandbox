#ifndef LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_

#include <optional>
#include <filesystem>
#include <cinttypes>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

#include <vulkan/vulkan.hpp>

#include <fmt/format.h>

#include <libsbx/graphics/buffers/buffer.hpp>
#include <libsbx/graphics/buffers/uniform_handler.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/graphics/descriptor/descriptor.hpp>
#include <libsbx/graphics/descriptor/descriptor_set.hpp>

#include <libsbx/graphics/images/image2d.hpp>

namespace sbx::graphics {

enum class polygon_mode : std::uint8_t {
  fill = VK_POLYGON_MODE_FILL,
  line = VK_POLYGON_MODE_LINE,
  point = VK_POLYGON_MODE_POINT
}; // enum class polygon_mode

enum class cull_mode : std::uint8_t {
  none = VK_CULL_MODE_NONE,
  front = VK_CULL_MODE_FRONT_BIT,
  back = VK_CULL_MODE_BACK_BIT,
  front_and_back = VK_CULL_MODE_FRONT_AND_BACK
}; // enum class cull_mode

enum class front_face : std::uint8_t {
  counter_clockwise = VK_FRONT_FACE_COUNTER_CLOCKWISE,
  clockwise = VK_FRONT_FACE_CLOCKWISE
}; // enum class front_face

struct depth_bias {
  std::float_t constant_factor{0.0f};
  std::float_t clamp{0.0f};
  std::float_t slope_factor{0.0f};
}; // struct depth_bias

struct rasterization_state {
  graphics::polygon_mode polygon_mode{polygon_mode::fill};
  std::float_t line_width{1.0f};
  graphics::cull_mode cull_mode{cull_mode::back};
  graphics::front_face front_face{front_face::counter_clockwise};
  std::optional<graphics::depth_bias> depth_bias{};
}; // struct rasterization_state

enum class primitive_topology : std::uint8_t {
  point_list = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
  line_list = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
  line_strip = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
  triangle_list = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  triangle_strip = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
  triangle_fan = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
  line_list_with_adjacency = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
  line_strip_with_adjacency = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
  triangle_list_with_adjacency = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
  triangle_strip_with_adjacency = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
  patch_list = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
}; // enum class primitive_topology

struct pipeline_definition {
  bool uses_depth{true};
  bool uses_transparency{false};
  graphics::rasterization_state rasterization_state{};
  graphics::primitive_topology primitive_topology{graphics::primitive_topology::triangle_list};
}; // struct pipeline_definition

template<vertex Vertex>
class graphics_pipeline : public pipeline {

public:

  using vertex_type = Vertex;

  graphics_pipeline(const std::filesystem::path& path, const pipeline::stage& stage, const pipeline_definition& definition = pipeline_definition{});

  ~graphics_pipeline() override;

  auto handle() const noexcept -> const VkPipeline& override;

  auto has_variable_descriptors() const noexcept -> bool override {
    return _has_variable_descriptors;
  }

  auto descriptor_counts() const noexcept -> const std::unordered_map<std::uint32_t, std::uint32_t>& override {
    return _descriptor_count_at_binding;
  }

  auto descriptor_set_layout() const noexcept -> const VkDescriptorSetLayout& override;

  auto descriptor_pool() const noexcept -> const VkDescriptorPool& override;

  auto layout() const noexcept -> const VkPipelineLayout& override;

  auto bind_point() const noexcept -> VkPipelineBindPoint override;

  auto stage() const noexcept -> const pipeline::stage& {
    return _stage;
  }

  auto descriptor_block(const std::string& name) const -> const shader::uniform_block& override {
    if (auto it = _uniform_blocks.find(name); it != _uniform_blocks.end()) {
      return it->second;
    }

    throw std::runtime_error(fmt::format("Failed to find descriptor block '{}' in graphics pipeline '{}'", name, _name));
  }

  auto find_descriptor_binding(const std::string& name) const -> std::optional<std::uint32_t> override {
    if (auto it = _descriptor_bindings.find(name); it != _descriptor_bindings.end()) {
      return it->second;
    }

    return std::nullopt;
  }

  auto find_descriptor_type_at_binding(std::uint32_t binding) const -> std::optional<VkDescriptorType> override {
    if (auto it = _descriptor_type_at_binding.find(binding); it != _descriptor_type_at_binding.end()) {
      return it->second;
    }

    return std::nullopt;
  }

private:

  auto _get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits;

  std::unordered_map<VkShaderStageFlagBits, std::unique_ptr<shader>> _shaders{};

  std::unordered_map<std::string, shader::uniform> _uniforms{};
  std::unordered_map<std::string, shader::uniform_block> _uniform_blocks{};

  std::unordered_map<std::uint32_t, VkDescriptorType> _descriptor_type_at_binding{};
  std::unordered_map<std::uint32_t, std::uint32_t> _descriptor_count_at_binding{};

  std::unordered_map<std::string, std::uint32_t> _descriptor_bindings{};
  std::unordered_map<std::string, std::uint32_t> _descriptor_sizes{};

  std::string _name{};
  VkPipelineLayout _layout{};
  VkPipeline _handle{};
  VkPipelineBindPoint _bind_point{};
  bool _has_variable_descriptors;

  pipeline::stage _stage{};

  VkDescriptorPool _descriptor_pool{};
  VkDescriptorSetLayout _descriptor_set_layout{};

}; // class graphics_pipeline

} // namespace sbx::graphics

#include <libsbx/graphics/pipeline/graphics_pipeline.ipp>

#endif // LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_
