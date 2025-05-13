#ifndef LIBSBX_GRAPHICS_COMPUTE_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_COMPUTE_PIPELINE_HPP_

#include <vulkan/vulkan.hpp>

#include <fmt/format.h>

#include <libsbx/math/vector3.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>

namespace sbx::graphics {

class compute_pipeline : public pipeline {

public:

  compute_pipeline(const std::filesystem::path& path);

  ~compute_pipeline() override;

  auto handle() const noexcept -> VkPipeline override {
    return _handle;
  }

  auto has_variable_descriptors() const noexcept -> bool override {
    return false;
  }

  auto descriptor_counts(const std::uint32_t set) const noexcept -> const std::vector<std::uint32_t>& override {
    return _descriptor_count_in_set[set];
  }

  auto descriptor_set_layouts() const noexcept -> const std::vector<VkDescriptorSetLayout>& override {
    return _descriptor_set_layouts;
  }

  auto descriptor_set_layout(const std::uint32_t set) const noexcept -> VkDescriptorSetLayout override {
    return _descriptor_set_layouts[set];
  }

  auto descriptor_pool() const noexcept -> VkDescriptorPool override {
    return _descriptor_pool;
  }

  auto layout() const noexcept -> VkPipelineLayout override {
    return _layout;
  }

  auto bind_point() const noexcept -> VkPipelineBindPoint override {
    return _bind_point;
  }

  auto descriptor_block(const std::string& name) const -> std::optional<shader::uniform_block> override {
    if (auto it = _uniform_blocks.find(name); it != _uniform_blocks.end()) {
      return it->second;
    }

    return std::nullopt;
  }

  auto find_descriptor_binding(const std::string& name) const -> std::optional<descriptor_id> override {
    if (auto it = _descriptor_bindings.find(name); it != _descriptor_bindings.end()) {
      return it->second;
    }

    return std::nullopt;
  }

  auto find_descriptor_type_at_binding(const descriptor_id& binding) const -> std::optional<VkDescriptorType> override {
    if (auto it = _descriptor_type_at_binding.find(binding); it != _descriptor_type_at_binding.end()) {
      return it->second;
    }

    return std::nullopt;
  }

  auto dispatch(command_buffer& command_buffer, const math::vector3u& groups) -> void {
    vkCmdDispatch(command_buffer, groups.x(), groups.y(), groups.z());
  }

private:

  auto _get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits;

  std::unique_ptr<shader> _shader;

  std::unordered_map<std::string, shader::uniform> _uniforms;
  std::unordered_map<std::string, shader::uniform_block> _uniform_blocks;

  std::unordered_map<descriptor_id, VkDescriptorType> _descriptor_type_at_binding;
  std::vector<std::vector<std::uint32_t>> _descriptor_count_in_set{};

  std::unordered_map<std::string, descriptor_id> _descriptor_bindings;
  std::unordered_map<std::string, std::uint32_t> _descriptor_sizes;

  std::string _name;
  VkPipelineLayout _layout;
  VkPipeline _handle;
  VkPipelineBindPoint _bind_point;

  VkDescriptorPool _descriptor_pool;
  std::vector<VkDescriptorSetLayout> _descriptor_set_layouts;

}; // class compute_pipeline

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_COMPUTE_PIPELINE_HPP_
