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

// [TODO] KAJ 2022-07-21 04:21 - Redesign buffer class. This is to general... May create explicit vertex_buffer and index_buffer class

enum class buffer_type : sbx::uint32 {
  vertex_buffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  index_buffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT
};

template<typename Type, std::size_t Size, buffer_type BufferType>
class buffer : sbx::noncopyable {

public:

  using value_type = Type;
  using size_type = std::size_t;

  buffer() = default;

  buffer(physical_device* physical_device, logical_device* logical_device, command_pool* command_pool, const std::array<value_type, Size>& data)
  : _physical_device{physical_device},
    _logical_device{logical_device},
    _command_pool{command_pool} {
    _initialize(data);
  }

  ~buffer() {
    _terminate();
  }

  VkBuffer handle() const {
    return _buffer_data.buffer;
  } 

  [[nodiscard]] size_type size() const noexcept {
    return Size;
  }

private:

  struct buffer_data {
    VkBuffer buffer{};
    VkDeviceMemory memory{};
  };

  void _initialize(const std::array<value_type, Size>& data) {
    const auto buffer_size = sizeof(value_type) * Size;

    auto staging_buffer_data = _create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    _map_memory(staging_buffer_data, data, buffer_size);

    _buffer_data = _create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | static_cast<std::underlying_type_t<buffer_type>>(BufferType), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    _copy_buffer(staging_buffer_data.buffer, _buffer_data.buffer, buffer_size);

    _destroy_buffer(staging_buffer_data);
  }

  void _map_memory(buffer_data& target, const std::array<value_type, Size>& data,  const VkDeviceSize size) const {
    auto data_buffer = static_cast<void*>(nullptr);

    vkMapMemory(_logical_device->handle(), target.memory, 0, size, 0, &data_buffer);
    std::memcpy(data_buffer, data.data(), size);
    vkUnmapMemory(_logical_device->handle(), target.memory);
  }

  buffer_data _create_buffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties) const {
    auto buffer_handle = VkBuffer{};
    auto memory_handle = VkDeviceMemory{};

    const auto buffer_create_info = VkBufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = size,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr
    };

    if (vkCreateBuffer(_logical_device->handle(), &buffer_create_info, nullptr, &buffer_handle) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create buffer!");
    }

    auto memory_requirements = VkMemoryRequirements{};
    vkGetBufferMemoryRequirements(_logical_device->handle(), buffer_handle, &memory_requirements);

    const auto memory_allocate_info = VkMemoryAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = _physical_device->find_memory_type(memory_requirements.memoryTypeBits, properties)
    };

    if (vkAllocateMemory(_logical_device->handle(), &memory_allocate_info, nullptr, &memory_handle) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate buffer memory");
    }

    if (vkBindBufferMemory(_logical_device->handle(), buffer_handle, memory_handle, 0) != VK_SUCCESS) {
      throw std::runtime_error("Failed to bind buffer memory");
    }

    return buffer_data{buffer_handle, memory_handle};
  }

  void _destroy_buffer(buffer_data& data) const {
    vkFreeMemory(_logical_device->handle(), data.memory, nullptr);
    vkDestroyBuffer(_logical_device->handle(), data.buffer, nullptr);
  }

  void _copy_buffer(const VkBuffer source_buffer, const VkBuffer destination_buffer, const VkDeviceSize size) const {
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

    vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &buffer_copy);

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

  void _terminate() {
    _destroy_buffer(_buffer_data);
  }

  physical_device* _physical_device{};
  logical_device* _logical_device{};
  command_pool* _command_pool{};

  buffer_data _buffer_data{};

}; // class buffer

} // namespace demo

#endif // DEMO_BUFFER_HPP_
