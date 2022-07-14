#ifndef DEMO_BUFFER_HPP_
#define DEMO_BUFFER_HPP_

#include <array>
#include <cstring>

#include <vulkan/vulkan.hpp>

#include <utils/noncopyable.hpp>

#include "physical_device.hpp"
#include "logical_device.hpp"

namespace demo {

template<typename Type, std::size_t Size>
class buffer : sbx::noncopyable {

public:

  using value_type = Type;
  using size_type = std::size_t;

  buffer(physical_device* physical_device, logical_device* logical_device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
  : _physical_device{physical_device},
    _logical_device{logical_device},
    _handle{nullptr},
    _memory{nullptr} {
    _initialize(usage, properties);
  }

  ~buffer() {
    _terminate();
  }

  VkBuffer handle() const {
    return _handle;
  }

  void map(const std::array<value_type, Size>& data) {
    const auto size = sizeof(value_type) * Size;

    void* memory = nullptr;
    vkMapMemory(_logical_device->handle(), _memory, 0, size, 0, &memory);
    std::memcpy(memory, data.data(), size);
    vkUnmapMemory(_logical_device->handle(), _memory);
  }

private:

  void _initialize(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    const auto buffer_create_info = VkBufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = sizeof(value_type) * Size,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr
    };

    if (vkCreateBuffer(_logical_device->handle(), &buffer_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create buffer");
    }

    auto memory_requirements = VkMemoryRequirements{};
    vkGetBufferMemoryRequirements(_logical_device->handle(), _handle, &memory_requirements);

    const auto memory_allocate_info = VkMemoryAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = _physical_device->find_memory_type(memory_requirements.memoryTypeBits, properties)
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

  VkBuffer _handle{};
  VkDeviceMemory _memory{};

}; // class buffer

} // namespace demo

#endif // DEMO_BUFFER_HPP_
