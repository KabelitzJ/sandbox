#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/core/assert.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::graphics {

buffer::buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, memory::observer_ptr<void> memory)
: _size{size} {
  const auto& physical_device = graphics_module::get().physical_device();
  const auto& logical_device = graphics_module::get().logical_device();

  const auto& sharing_mode = logical_device.queue_sharing_mode();

  auto buffer_create_info = VkBufferCreateInfo{};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = _size;
  buffer_create_info.usage = usage;
  buffer_create_info.sharingMode = sharing_mode.mode;
  buffer_create_info.queueFamilyIndexCount = static_cast<std::uint32_t>(sharing_mode.queue_families.size());
  buffer_create_info.pQueueFamilyIndices = sharing_mode.queue_families.data();

  validate(vkCreateBuffer(logical_device, &buffer_create_info, nullptr, &_handle));

  auto memory_requirements = VkMemoryRequirements{};
  vkGetBufferMemoryRequirements(logical_device, _handle, &memory_requirements);

  auto allocation_info = VkMemoryAllocateInfo{};
  allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocation_info.allocationSize = memory_requirements.size;
  allocation_info.memoryTypeIndex = physical_device.find_memory_type(memory_requirements.memoryTypeBits, properties);

  validate(vkAllocateMemory(logical_device, &allocation_info, nullptr, &_memory));

  if (memory) {
    auto mapped_memory = map();

    std::memcpy(mapped_memory.get(), memory.get(), _size);

    if (!(usage & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
      auto flush_range = VkMappedMemoryRange{};
      flush_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      flush_range.memory = _memory;
      flush_range.offset = 0;
      flush_range.size = _size;

      validate(vkFlushMappedMemoryRanges(logical_device, 1, &flush_range));
    }

    unmap();
  }

  validate(vkBindBufferMemory(logical_device, _handle, _memory, 0));
}

buffer::~buffer() {
  const auto& logical_device = graphics_module::get().logical_device();

  vkFreeMemory(logical_device, _memory, nullptr);
  vkDestroyBuffer(logical_device, _handle, nullptr);
}

auto buffer::handle() const noexcept -> const VkBuffer& {
  return _handle;
}

buffer::operator const VkBuffer&() const noexcept {
  return _handle;
}

auto buffer::memory() const noexcept -> const VkDeviceMemory& {
  return _memory;
}

auto buffer::size() const noexcept -> std::size_t {
  return _size;
}

auto buffer::map() -> memory::observer_ptr<void> {
  const auto& logical_device = graphics_module::get().logical_device();

  auto* mapped_memory = static_cast<void*>(nullptr);

  validate(vkMapMemory(logical_device, _memory, 0, _size, 0, &mapped_memory));

  return memory::observer_ptr<void>{mapped_memory};
}

auto buffer::unmap() -> void {
  const auto& logical_device = graphics_module::get().logical_device();

  vkUnmapMemory(logical_device, _memory);
}

auto buffer::write(const void* data, VkDeviceSize size, VkDeviceSize offset) -> void {
  const auto& logical_device = graphics_module::get().logical_device();

  auto mapped_memory = map();

  std::memcpy(static_cast<std::uint8_t*>(mapped_memory.get()) + offset, data, size);

  unmap();
}

} // namespace sbx::graphics
