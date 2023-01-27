#ifndef LIBSBX_GRAPHICS_COMMANDS_COMMAND_BUFFER_HPP_
#define LIBSBX_GRAPHICS_COMMANDS_COMMAND_BUFFER_HPP_

#include <memory>

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/commands/command_pool.hpp>

namespace sbx::graphics {

class command_buffer {

  static constexpr auto submit_pipeline_stages = VkPipelineStageFlags{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

public:

  command_buffer(bool should_begin = true, VkQueueFlagBits queue_type = VK_QUEUE_GRAPHICS_BIT, VkCommandBufferLevel buffer_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

  ~command_buffer();

  auto handle() const noexcept -> const VkCommandBuffer&;

  operator const VkCommandBuffer&() const noexcept;

  auto is_running() const noexcept -> bool;

  auto begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) -> void;

  auto end() -> void;

  auto submit_idle() -> void;

  auto submit(const VkSemaphore& wait_semaphore = nullptr, const VkSemaphore &signal_semaphore = nullptr, const VkFence& fence = nullptr) -> void;

private:

  auto _queue() const noexcept -> VkQueue;

  std::shared_ptr<command_pool> _command_pool{};

  VkCommandBuffer _handle{};
  VkQueueFlagBits _queue_type{};
  bool _is_running{};

}; // class command_buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_COMMANDS_COMMAND_BUFFER_HPP_
