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

#include <libsbx/utility/noncopyable.hpp>
#include <libsbx/utility/enable_private_constructor.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>
#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/graphics/descriptor/descriptor.hpp>
#include <libsbx/graphics/descriptor/descriptor_set.hpp>

namespace sbx::graphics {

class graphics_pipeline : public pipeline {

public:

  graphics_pipeline(stage stage, const std::filesystem::path& path, const vertex_input_description& vertex_input_description);

  ~graphics_pipeline() override;

  auto handle() const noexcept -> const VkPipeline& override;

  auto descriptor_set_layout() const noexcept -> const VkDescriptorSetLayout& override;

  auto descriptor_pool() const noexcept -> const VkDescriptorPool& override;

  auto layout() const noexcept -> const VkPipelineLayout& override;

  auto bind_point() const noexcept -> VkPipelineBindPoint override;

  auto find_descriptor_block(const std::string& name) const noexcept -> std::optional<shader::uniform_block> {
    if (auto it = _uniform_blocks.find(name); it != _uniform_blocks.end()) {
      return it->second;
    }

    return std::nullopt;
  }

  auto push(const std::string name, uniform_handler& uniform) -> void;

  auto bind_descriptors(const command_buffer& command_buffer) -> void;

private:

  struct descriptor_entry {
    const descriptor* descriptor;
    write_descriptor_set write_descriptor_set;
    std::uint32_t binding;
  }; // struct descriptor_entry

  auto _get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits;

  template<typename Descriptor>
  requires (std::is_base_of_v<descriptor, Descriptor>)
  auto _push(const std::string& name, const Descriptor& descriptor) -> void {
    auto binding_entry = _descriptor_bindings.find(name);

    if (binding_entry == _descriptor_bindings.end()) {
      throw std::runtime_error(fmt::format("Failed to find descriptor binding for descriptor '{}'", name));
    }

    auto binding = binding_entry->second;

    auto descriptor_type_entry = _descriptor_type_at_binding.find(binding);

    if (descriptor_type_entry == _descriptor_type_at_binding.end()) {
      throw std::runtime_error(fmt::format("Failed to find descriptor type for descriptor '{}'", name));
    }

    auto descriptor_type = descriptor_type_entry->second;

    auto write_descriptor_set = descriptor.write_descriptor_set(binding, descriptor_type);

    _descriptors.insert({name, descriptor_entry{std::addressof(descriptor), std::move(write_descriptor_set), binding}});

    _is_descriptor_set_dirty = true;
  }

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

  stage _stage{};

  VkDescriptorPool _descriptor_pool{};
  VkDescriptorSetLayout _descriptor_set_layout{};

  std::vector<std::unique_ptr<descriptor_set>> _descriptor_sets{};

  std::map<std::string, descriptor_entry> _descriptors{};
  std::vector<VkWriteDescriptorSet> _write_descriptor_sets{};
  bool _is_descriptor_set_dirty{};

}; // class graphics_pipeline

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_
