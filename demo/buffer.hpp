#ifndef DEMO_BUFFER_HPP_
#define DEMO_BUFFER_HPP_

#include <array>
#include <cstring>

#include <vulkan/vulkan.hpp>

#include <utils/noncopyable.hpp>

#include <types/primitives.hpp>

#include "physical_device.hpp"
#include "logical_device.hpp"
#include "command_pool.hpp"

namespace demo {

template<typename Type>
class buffer : sbx::noncopyable {

public:

  using value_type = Type;

  buffer() = default;

  buffer(physical_device* physical_device, logical_device* logical_device, command_pool* command_pool, const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties)
  : _physical_device{physical_device},
    _logical_device{logical_device},
    _command_pool{command_pool},
    _size{size},
    _usage{usage},
    _properties{properties} {
    _initialize();
  }

  ~buffer() {
    _terminate();
  }

  [[nodiscard]] VkBuffer handle() const noexcept {
    return _handle;
  } 

  [[nodiscard]] VkDeviceSize size() const noexcept {
    return _size;
  }

  void map(const std::vector<value_type>& data) {
    if (data.size() != _size) {
      throw std::runtime_error{"Data could not be mapped"};
    }

    if (!(_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
      throw std::runtime_error{"Cannot call map on buffer that is not a staging buffer"};
    }

    const auto size = sizeof(value_type) * _size;

    auto data_buffer = static_cast<void*>(nullptr);

    vkMapMemory(_logical_device->handle(), _memory, 0, size, 0, &data_buffer);
    std::memcpy(data_buffer, data.data(), size);
    vkUnmapMemory(_logical_device->handle(), _memory);
  }

  void copy_from(const buffer& other) {
    if (_size != other._size) {
      throw std::runtime_error{"Data could not be mapped"};
    }

    if (!(_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
      throw std::runtime_error("Buffer is not a transfer destination");
    }

    if (!(other._usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
      throw std::runtime_error("Given buffer is not a transfer source");
    }

    const auto size = sizeof(value_type) * _size;

    const auto command_buffer_allocation_info = VkCommandBufferAllocateInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = _command_pool->handle(),
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1
    };

    auto command_buffer = VkCommandBuffer{};
    vkAllocateCommandBuffers(_logical_device->handle(), &command_buffer_allocation_info, &command_buffer);

    const auto command_buffer_begin_info = VkCommandBufferBeginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr
    };

    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    const auto buffer_copy = VkBufferCopy{
      .srcOffset = 0,
      .dstOffset = 0,
      .size = size
    };

    vkCmdCopyBuffer(command_buffer, other._handle, _handle, 1, &buffer_copy);

    vkEndCommandBuffer(command_buffer);

    const auto submit_info = VkSubmitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 0,
      .pWaitSemaphores = nullptr,
      .pWaitDstStageMask = nullptr,
      .commandBufferCount = 1,
      .pCommandBuffers = &command_buffer,
      .signalSemaphoreCount = 0,
      .pSignalSemaphores = nullptr
    };

    vkQueueSubmit(_logical_device->graphics_queue(), 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(_logical_device->graphics_queue());

    vkFreeCommandBuffers(_logical_device->handle(), _command_pool->handle(), 1, &command_buffer);
  }

private:

  void _initialize() {
    const auto size = sizeof(value_type) * _size;

    const auto buffer_create_info = VkBufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = size,
      .usage = _usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr
    };

    if (vkCreateBuffer(_logical_device->handle(), &buffer_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create buffer!");
    }

    auto memory_requirements = VkMemoryRequirements{};
    vkGetBufferMemoryRequirements(_logical_device->handle(), _handle, &memory_requirements);

    const auto memory_allocate_info = VkMemoryAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = _physical_device->find_memory_type(memory_requirements.memoryTypeBits, _properties)
    };

    if (vkAllocateMemory(_logical_device->handle(), &memory_allocate_info, nullptr, &_memory) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate buffer memory");
    }

    if (vkBindBufferMemory(_logical_device->handle(), _handle, _memory, 0) != VK_SUCCESS) {
      throw std::runtime_error("Failed to bind buffer memory");
    }
  }

  void _terminate() {
    vkFreeMemory(_logical_device->handle(), _memory, nullptr);
    vkDestroyBuffer(_logical_device->handle(), _handle, nullptr);
  }

  physical_device* _physical_device{};
  logical_device* _logical_device{};
  command_pool* _command_pool{};

  VkDeviceSize _size{};
  VkBufferUsageFlags _usage{};
  VkMemoryPropertyFlags _properties{};
  VkBuffer _handle{};
  VkDeviceMemory _memory{};

}; // class buffer

} // namespace demo

#endif // DEMO_BUFFER_HPP_
