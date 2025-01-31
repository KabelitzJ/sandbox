#include <libsbx/graphics/commands/command_buffer.hpp>

#include <limits>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/assert.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

inline static constexpr auto submit_pipeline_stages = VkPipelineStageFlags{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

command_buffer::command_buffer(bool should_begin, VkQueueFlagBits queue_type, VkCommandBufferLevel buffer_level)
: _queue_type{queue_type},
  _is_running{false} {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  _command_pool = graphics_module.command_pool(queue_type);

  auto command_buffer_allocate_info = VkCommandBufferAllocateInfo{};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = *_command_pool;
	command_buffer_allocate_info.level = buffer_level;
	command_buffer_allocate_info.commandBufferCount = 1;

  validate(vkAllocateCommandBuffers(logical_device, &command_buffer_allocate_info, &_handle));

  if (should_begin) {
    begin();
  }
}

command_buffer::~command_buffer() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();
  vkFreeCommandBuffers(logical_device, *_command_pool, 1, &_handle);
}

auto command_buffer::handle() const noexcept -> const VkCommandBuffer& {
  return _handle;
}

command_buffer::operator const VkCommandBuffer&() const noexcept {
  return _handle;
}

auto command_buffer::is_running() const noexcept -> bool {
  return _is_running;
}

auto command_buffer::begin(VkCommandBufferUsageFlags usage) -> void {
  if (_is_running) {
    core::logger::warn("Tried to begin recording a command buffer that was already beeing recorded");
    return;
  }

  auto command_buffer_begin_info = VkCommandBufferBeginInfo{};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = usage;

	validate(vkBeginCommandBuffer(_handle, &command_buffer_begin_info));

	_is_running = true;
}

auto command_buffer::end() -> void {
  if (!_is_running) {
    core::logger::warn("Tried to stop recording a command buffer that was not beeing recorded");
    return;
  }

  validate(vkEndCommandBuffer(_handle));

  _is_running = false;
}

auto command_buffer::submit_idle() -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();
	const auto& selected_queue = _queue();

	if (_is_running) {
		end();
  }

	auto submit_info = VkSubmitInfo{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_handle;

	auto fence_create_info = VkFenceCreateInfo{};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	auto fence = VkFence{};

	validate(vkCreateFence(logical_device, &fence_create_info, nullptr, &fence));

	validate(vkQueueSubmit(selected_queue, 1, &submit_info, fence));

	validate(vkWaitForFences(logical_device, 1, &fence, true, std::numeric_limits<std::uint64_t>::max()));

	vkDestroyFence(logical_device, fence, nullptr);
}

auto command_buffer::submit(const std::vector<wait_data>& wait_data, const VkSemaphore &signal_semaphore, const VkFence& fence) -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();
	const auto& selected_queue = _queue();

	if (_is_running) {
		end();
  }

  auto submit_info = VkSubmitInfo{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_handle;
  submit_info.pWaitDstStageMask = &submit_pipeline_stages;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &wait_semaphore;
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &signal_semaphore;

  auto wait_stages = std::vector<VkPipelineStageFlags>{};
  auto wait_semaphores = std::vector<VkSemaphore>{};

  if (!wait_data.empty()) {
    wait_semaphores.reserve(wait_data.size());
    wait_stages.reserve(wait_data.size());

    for (const auto& data : wait_data) {
      wait_semaphores.push_back(data.semaphore);
      wait_stages.push_back(data.stage);
    }

		submit_info.waitSemaphoreCount = static_cast<std::uint32_t>(wait_data.size());
		submit_info.pWaitSemaphores = wait_semaphores.data();
		submit_info.pWaitDstStageMask = wait_stages.data();
	}

	if (signal_semaphore) {
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &signal_semaphore;
	}

	if (fence) {
		validate(vkResetFences(logical_device, 1, &fence));
  }

	validate(vkQueueSubmit(selected_queue, 1, &submit_info, fence));
}

auto command_buffer::copy_buffer(const VkBuffer& source, const VkBuffer& destination, const VkBufferCopy& region) -> void {
  vkCmdCopyBuffer(_handle, source, destination, 1, &region);
}

auto command_buffer::set_viewport(const VkViewport& viewport) -> void {
  vkCmdSetViewport(_handle, 0, 1, &viewport);
}

auto command_buffer::set_scissor(const VkRect2D& scissor) -> void {
  vkCmdSetScissor(_handle, 0, 1, &scissor); 
}

// auto command_buffer::bind_pipeline(const pipeline& pipeline, VkPipelineBindPoint bind_point) -> void {
//   vkCmdBindPipeline(_handle, bind_point, pipeline);
// }

auto command_buffer::bind_vertex_buffer(std::uint32_t first_binding, const VkBuffer& buffer) -> void {
  auto buffers = std::array<VkBuffer, 1>{buffer};
  auto offsets = std::array<VkDeviceSize, 1>{0};

  vkCmdBindVertexBuffers(_handle, first_binding, 1, buffers.data(), offsets.data());
}

auto command_buffer::bind_index_buffer(const VkBuffer& buffer, VkDeviceSize offset, VkIndexType index_type) -> void {
  vkCmdBindIndexBuffer(_handle, buffer, offset, index_type);
}

auto command_buffer::draw(std::uint32_t vertex_count, std::uint32_t instance_count, std::uint32_t first_vertex, std::uint32_t first_instance) -> void {
  vkCmdDraw(_handle, vertex_count, instance_count, first_vertex, first_instance);
}

auto command_buffer::draw_indexed(std::uint32_t index_count, std::uint32_t instance_count, std::uint32_t first_index, std::int32_t vertex_offset, std::uint32_t first_instance) -> void {
  vkCmdDrawIndexed(_handle, index_count, instance_count, first_index, vertex_offset, first_instance);
}

// auto command_buffer::push_constants(const pipeline& pipeline, VkShaderStageFlags stage_flags, const push_constant& constants) -> void {
//   vkCmdPushConstants(_handle, pipeline.layout(), stage_flags, 0, sizeof(push_constant), &constants);
// }

// auto command_buffer::bind_descriptor_set(const pipeline& pipeline, VkPipelineBindPoint bind_point) -> void {
//   vkCmdBindDescriptorSets(_handle, bind_point, pipeline.layout(), 0, 1, &pipeline.active_descriptor_set(), 0, nullptr);
// }

auto command_buffer::begin_render_pass(const VkRenderPassBeginInfo& renderpass_begin_info, VkSubpassContents subpass_contents) -> void {
  vkCmdBeginRenderPass(_handle, &renderpass_begin_info, subpass_contents);
}

auto command_buffer::end_render_pass() -> void {
  vkCmdEndRenderPass(_handle);
}

auto command_buffer::_queue() const -> const graphics::queue& {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  switch (_queue_type) {
    case VK_QUEUE_GRAPHICS_BIT: {
      return logical_device.queue<graphics::queue::type::graphics>();
    }
    case VK_QUEUE_COMPUTE_BIT: {
      return logical_device.queue<graphics::queue::type::compute>();
    }
    case VK_QUEUE_TRANSFER_BIT: {
      return logical_device.queue<graphics::queue::type::transfer>();
    }
    default: {
      throw std::runtime_error{"Invalid queue type"};
    }
  }
}

} // namespace sbx::graphics
