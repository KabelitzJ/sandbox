#ifndef LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_

#include <cinttypes>
#include <optional>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>
#include <libsbx/utility/hash.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>

namespace sbx::graphics {

class pipeline : public utility::noncopyable {

public:

  struct stage {
    std::uint32_t renderpass;
    std::uint32_t subpass;

    auto operator==(const stage& rhs) const noexcept -> bool {
      return renderpass == rhs.renderpass && subpass == rhs.subpass;
    }

    auto operator<(const stage& rhs) const noexcept -> bool {
      return renderpass < rhs.renderpass || (renderpass == rhs.renderpass && subpass < rhs.subpass);
    }
  }; // struct stage 

  struct descriptor_id {
    std::uint32_t set;
    std::uint32_t binding;

    auto operator==(const descriptor_id& rhs) const noexcept -> bool {
      return set == rhs.set && binding == rhs.binding;
    }

    auto operator<(const descriptor_id& rhs) const noexcept -> bool {
      return set < rhs.set || (set == rhs.set && binding < rhs.binding);
    }
  }; // struct descriptor_id

  inline static constexpr auto descriptor_sets = std::uint32_t{4};

  pipeline() = default;

  virtual ~pipeline() = default;

  auto bind(command_buffer& command_buffer) const noexcept -> void {
    vkCmdBindPipeline(command_buffer, bind_point(), handle());
  }

  operator VkPipeline() const noexcept {
    return handle();
  }

  virtual auto handle() const noexcept -> VkPipeline = 0;

  virtual auto has_variable_descriptors() const noexcept -> bool = 0;

  virtual auto descriptor_counts(const std::uint32_t set) const noexcept -> const std::vector<std::uint32_t>& = 0;

  /**
   * VkDescriptorSetLayout for each used set int he shader.
   * 
   * @note The size of vector is the biggest set + 1 and sets that are no used are VK_NULL_HANDLE
   * So if a shader uses sets 0 and 2, the vector will contain [VkDescriptorSetLayout, VK_NULL_HANDLE, VkDescriptorSetLayout]
   */
  virtual auto descriptor_set_layouts() const noexcept -> const std::vector<VkDescriptorSetLayout>& = 0;

  virtual auto descriptor_set_layout(const std::uint32_t set) const noexcept -> VkDescriptorSetLayout = 0;

  virtual auto descriptor_pool() const noexcept -> VkDescriptorPool = 0;

  virtual auto layout() const noexcept -> VkPipelineLayout = 0;

  virtual auto bind_point() const noexcept -> VkPipelineBindPoint = 0;

  virtual auto descriptor_block(const std::string& name) const -> std::optional<shader::uniform_block> = 0;

  virtual auto find_descriptor_binding(const std::string& name) const -> std::optional<descriptor_id> = 0;

  virtual auto find_descriptor_type_at_binding(const descriptor_id& binding) const -> std::optional<VkDescriptorType> = 0;

}; // class pipeline

} // namespace sbx::graphics

template<>
struct std::hash<sbx::graphics::pipeline::stage> {
  auto operator()(const sbx::graphics::pipeline::stage& stage) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, stage.renderpass, stage.subpass);
    return hash;
  }
}; // struct std::hash

template<>
struct std::hash<sbx::graphics::pipeline::descriptor_id> {
  auto operator()(const sbx::graphics::pipeline::descriptor_id& descriptor_id) const noexcept -> std::size_t {
    auto hash = std::size_t{0};
    sbx::utility::hash_combine(hash, descriptor_id.set, descriptor_id.binding);
    return hash;
  }
}; // struct std::hash


#endif // LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
