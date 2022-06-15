#ifndef DEMO_COMMAND_BUFFER_HPP_
#define DEMO_COMMAND_BUFFER_HPP_

#include <vulkan/vulkan.hpp>

#include "logical_device.hpp"
#include "command_pool.hpp"

namespace demo {

class command_buffer {

public:

  command_buffer(logical_device* logical_device, command_pool* command_pool)
  : _logical_device{logical_device},
    _command_pool{command_pool},
    _handle{nullptr} {
    _initialize();
  }

  command_buffer(const command_buffer&) = delete;

  command_buffer(command_buffer&&) = delete;

  ~command_buffer() {
    _terminate();
  }

  command_buffer& operator=(const command_buffer&) = delete;

  command_buffer& operator=(command_buffer&&) = delete;

  [[nodiscard]] VkCommandBuffer handle() const noexcept {
    return _handle;
  }

private:

  void _initialize() {
    const auto command_buffer_allocate_info = VkCommandBufferAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = _command_pool->handle(),
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1
    };

    if (vkAllocateCommandBuffers(_logical_device->handle(), &command_buffer_allocate_info, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to allocate command buffer"};
    }
  }

  void _terminate() {
    vkFreeCommandBuffers(_logical_device->handle(), _command_pool->handle(), 1, &_handle);
  }

  logical_device* _logical_device{};
  command_pool* _command_pool{};

  VkCommandBuffer _handle{};

}; // class command_buffer

} // namespace demo

#endif // DEMO_COMMAND_BUFFER_HPP_
