#ifndef LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <memory>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>
#include <libsbx/graphics/pipeline/uniform.hpp>

namespace sbx::graphics {

class pipeline : public utility::noncopyable {

public:

  pipeline(const std::filesystem::path& path);

  ~pipeline();

  auto handle() const noexcept -> const VkPipeline&;

  operator const VkPipeline&() const noexcept;

  auto layout() const noexcept -> const VkPipelineLayout&;

  auto active_descriptor_set() const noexcept -> const VkDescriptorSet&;

  auto update_uniform(const uniform& uniform) -> void;

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

}; // class pipeline

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
