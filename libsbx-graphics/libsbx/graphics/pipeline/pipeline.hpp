#ifndef LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_

#include <cinttypes>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::graphics {

class pipeline : public utility::noncopyable {

public:

  pipeline() = default;

  virtual ~pipeline() = default;

  auto bind(const command_buffer& command_buffer) const noexcept -> void {
    vkCmdBindPipeline(command_buffer, bind_point(), handle());
  }

  operator const VkPipeline&() const noexcept {
    return handle();
  }

  virtual auto handle() const noexcept -> const VkPipeline& = 0;

  virtual auto layout() const noexcept -> const VkPipelineLayout& = 0;

  virtual auto bind_point() const noexcept -> VkPipelineBindPoint = 0;

}; // class pipeline

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_PIPELINE_HPP_
