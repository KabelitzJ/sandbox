#include <libsbx/graphics/commands/command_buffer.hpp>

#include <limits>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/devices/logical_device.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

command_buffer::command_buffer(bool should_begin, VkQueueFlagBits queue_type, VkCommandBufferLevel buffer_level)
: _command_pool{graphics_module::get().command_pool()},
  _queue_type{queue_type},
  _is_running{false} {
  auto& logical_device = graphics_module::get().logical_device();

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
  auto& logical_device = graphics_module::get().logical_device();
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
  auto& logical_device = graphics_module::get().logical_device();
	auto selected_queue = _queue();

	if (_is_running) {
		end();
  }

	auto submit_info = VkSubmitInfo{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_handle;

	auto fence_create_info = VkFenceCreateInfo{};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	auto fence = VkFence{};

	validate(vkCreateFence(logical_device, &fence_create_info, nullptr, &fence));

  validate(vkResetFences(logical_device, 1, &fence));

	validate(vkQueueSubmit(selected_queue, 1, &submit_info, fence));

	validate(vkWaitForFences(logical_device, 1, &fence, true, std::numeric_limits<uint64_t>::max()));

	vkDestroyFence(logical_device, fence, nullptr);
}

auto command_buffer::submit(const VkSemaphore& wait_semaphore, const VkSemaphore &signal_semaphore, const VkFence& fence) -> void {
  auto& logical_device = graphics_module::get().logical_device();
	auto selected_queue = _queue();

	if (_is_running) {
		end();
  }

  auto submit_info = VkSubmitInfo{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_handle;

  if (wait_semaphore) {
		submit_info.pWaitDstStageMask = &submit_pipeline_stages;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &wait_semaphore;
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

auto command_buffer::_queue() const noexcept -> VkQueue {
  auto& logical_device = graphics_module::get().logical_device();

  if (_queue_type == VK_QUEUE_GRAPHICS_BIT) {
    return logical_device.graphics_queue().handle;
  } else if (_queue_type == VK_QUEUE_COMPUTE_BIT) {
    return logical_device.compute_queue().handle;
  }

  return nullptr;
}

} // namespace sbx::graphics