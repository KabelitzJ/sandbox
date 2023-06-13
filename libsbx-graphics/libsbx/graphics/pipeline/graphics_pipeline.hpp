#ifndef LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <memory>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>
#include <libsbx/utility/enable_private_constructor.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/uniform.hpp>
#include <libsbx/graphics/pipeline/push_constant.hpp>

namespace sbx::graphics {

class graphics_pipeline : public pipeline {

public:

  graphics_pipeline(const std::filesystem::path& path);

  ~graphics_pipeline() override;

  auto handle() const noexcept -> const VkPipeline& override;

  auto layout() const noexcept -> const VkPipelineLayout& override;

  auto bind_point() const noexcept -> VkPipelineBindPoint override;

  auto active_descriptor_set() const noexcept -> const VkDescriptorSet&;

  auto update_uniform(const uniform& uniform) -> void;

  auto update_push_constant(const command_buffer& command_buffer, VkShaderStageFlags stage_flags, const push_constant& data) const noexcept -> void {
    vkCmdPushConstants(command_buffer, layout(), stage_flags, 0, sizeof(push_constant), &data);
  }

  auto bind_descriptor_set(const command_buffer& command_buffer) const noexcept -> void {
    vkCmdBindDescriptorSets(command_buffer, bind_point(), layout(), 0, 1, &active_descriptor_set(), 0, nullptr);
  }

private:

  auto _get_stage_from_name(const std::string& name) const noexcept -> VkShaderStageFlagBits;

  std::unordered_map<VkShaderStageFlagBits, std::unique_ptr<shader>> _shaders{};

  std::string _name{};
  VkPipelineLayout _layout{};
  VkPipeline _handle{};

  VkDescriptorPool _descriptor_pool{};
  VkDescriptorSetLayout _descriptor_set_layout{};
  std::vector<VkDescriptorSet> _descriptor_sets{};
  std::vector<std::unique_ptr<buffer>> _uniform_buffers{};

}; // class graphics_pipeline

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_GRAPHICS_PIPELINE_HPP_
