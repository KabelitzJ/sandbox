#ifndef LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <optional>

#include <vulkan/vulkan.hpp>

#include <fmt/format.h>

#include <libsbx/assets/asset.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>
#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/graphics/descriptor/descriptor.hpp>
#include <libsbx/graphics/descriptor/descriptor_set.hpp>

#include <libsbx/graphics/images/image2d.hpp>

namespace sbx::graphics {

class graphics_pipeline : public pipeline, public assets::asset<assets::asset_type::pipeline> {

public:

  graphics_pipeline(const std::filesystem::path& path, const pipeline::stage& stage, const vertex_input_description& vertex_input_description);

  ~graphics_pipeline() override;

  auto handle() const noexcept -> const VkPipeline& override;

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

  std::map<std::string, shader::uniform> _uniforms{};
  std::map<std::string, shader::uniform_block> _uniform_blocks{};

  std::map<std::uint32_t, VkDescriptorType> _descriptor_type_at_binding{};

  std::map<std::string, std::uint32_t> _descriptor_bindings{};
  std::map<std::string, std::uint32_t> _descriptor_sizes{};

  std::string _name{};
  VkPipelineLayout _layout{};
  VkPipeline _handle{};
  VkPipelineBindPoint _bind_point{};

  pipeline::stage _stage{};

  VkDescriptorPool _descriptor_pool{};
  VkDescriptorSetLayout _descriptor_set_layout{};

}; // class graphics_pipeline

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_
