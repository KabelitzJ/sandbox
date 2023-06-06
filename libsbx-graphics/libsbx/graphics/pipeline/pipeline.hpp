#ifndef LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <memory>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>
#include <libsbx/utility/enable_private_constructor.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>
#include <libsbx/graphics/pipeline/uniform.hpp>
#include <libsbx/graphics/pipeline/vertex_input.hpp>

namespace sbx::graphics {

class pipeline : public utility::noncopyable {

public:

  ~pipeline();

  template<typename Vertex>
  static auto create(const std::filesystem::path& path) -> std::unique_ptr<pipeline> {
    auto binding_descriptions = vertex_input<Vertex>::binding_descriptions();
    auto attribute_descriptions = vertex_input<Vertex>::attribute_descriptions();

    return std::make_unique<utility::enable_private_constructor<pipeline>>(path, std::move(binding_descriptions), std::move(attribute_descriptions));
  }

  auto handle() const noexcept -> const VkPipeline&;

  operator const VkPipeline&() const noexcept;

  auto layout() const noexcept -> const VkPipelineLayout&;

  auto active_descriptor_set() const noexcept -> const VkDescriptorSet&;

  auto update_uniform(const uniform& uniform) -> void;

protected:

  pipeline(const std::filesystem::path& path, std::vector<VkVertexInputBindingDescription> binding_descriptions, std::vector<VkVertexInputAttributeDescription> attribute_descriptions);

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
