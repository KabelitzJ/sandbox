#ifndef LIBSBX_GRAPHICS_COMMANDS_COMMAND_BUFFER_HPP_
#define LIBSBX_GRAPHICS_COMMANDS_COMMAND_BUFFER_HPP_

#include <memory>

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/devices/logical_device.hpp>

#include <libsbx/graphics/commands/command_pool.hpp>

namespace sbx::graphics {

class command_buffer {

public:

  struct wait_data {
    VkSemaphore semaphore;
    VkPipelineStageFlags stage;
  }; // struct wait_data

  struct buffer_barrier_data {
    std::vector<VkBuffer> buffers;
    VkPipelineStageFlags src_stage_mask;
    VkPipelineStageFlags dst_stage_mask;
    VkAccessFlags src_access_mask;
    VkAccessFlags dst_access_mask;
  }; // struct buffer_barrier

  command_buffer(bool should_begin = true, VkQueueFlagBits queue_type = VK_QUEUE_GRAPHICS_BIT, VkCommandBufferLevel buffer_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

  command_buffer(const command_buffer&) = delete;

  command_buffer(command_buffer&&) noexcept;

  ~command_buffer();

  auto operator=(const command_buffer&) -> command_buffer& = delete;

  auto operator=(command_buffer&&) noexcept -> command_buffer&;

  auto handle() const noexcept -> const VkCommandBuffer&;

  operator const VkCommandBuffer&() const noexcept;

  auto is_running() const noexcept -> bool;

  auto begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) -> void;

  auto end() -> void;

  auto submit_idle() -> void;

  auto submit(const std::vector<wait_data>& wait_data = {}, const VkSemaphore &signal_semaphore = nullptr, const VkFence& fence = nullptr) -> void;

  auto copy_buffer(const VkBuffer& source, const VkBuffer& destination, const VkBufferCopy& region) -> void;

  auto buffer_barrier(const buffer_barrier_data& buffer_barrier_data) -> void;

  auto set_viewport(const VkViewport& viewport) -> void;

  auto set_scissor(const VkRect2D& scissor) -> void;

  auto bind_vertex_buffer(std::uint32_t first_binding, const VkBuffer& buffer) -> void;

  auto bind_index_buffer(const VkBuffer& buffer, VkDeviceSize offset, VkIndexType index_type) -> void;

  auto draw(std::uint32_t vertex_count, std::uint32_t instance_count, std::uint32_t first_vertex, std::uint32_t first_instance) -> void;

  auto draw_indexed(std::uint32_t index_count, std::uint32_t instance_count, std::uint32_t first_index, std::int32_t vertex_offset, std::uint32_t first_instance) -> void;

  auto begin_render_pass(const VkRenderPassBeginInfo& renderpass_begin_info, VkSubpassContents subpass_contents) -> void;

  auto end_render_pass() -> void;

private:

  auto _queue() const -> const graphics::queue&;

  std::shared_ptr<command_pool> _command_pool{};

  VkCommandBuffer _handle{};
  VkQueueFlagBits _queue_type{};
  bool _is_running{};

}; // class command_buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_COMMANDS_COMMAND_BUFFER_HPP_
