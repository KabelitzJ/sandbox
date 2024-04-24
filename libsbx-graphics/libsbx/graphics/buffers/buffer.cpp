#include <libsbx/graphics/buffers/buffer.hpp>

#include <fmt/format.h>

#include <libsbx/utility/assert.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::graphics {

buffer_base::buffer_base(size_type size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, memory::observer_ptr<const void> memory)
: _size{size} {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& physical_device = graphics_module.physical_device();
  const auto& logical_device = graphics_module.logical_device();

  const auto& graphics_queue = logical_device.queue<queue::type::graphics>();
  const auto& present_queue = logical_device.queue<queue::type::present>();
  const auto& compute_queue = logical_device.queue<queue::type::compute>();

  const auto queue_family_indices = std::array<std::uint32_t, 3>{graphics_queue.family(), present_queue.family(), compute_queue.family()};

  auto buffer_create_info = VkBufferCreateInfo{};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = _size;
  buffer_create_info.usage = usage;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  buffer_create_info.queueFamilyIndexCount = static_cast<std::uint32_t>(queue_family_indices.size());
  buffer_create_info.pQueueFamilyIndices = queue_family_indices.data();

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

    // [NOTE] KAJ 2023-07-28 : If the memory is not host coherent, we need to flush it.
    if (!(usage & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
      auto flush_range = VkMappedMemoryRange{};
      flush_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      flush_range.memory = _memory;
      flush_range.offset = 0;
      flush_range.size = VK_WHOLE_SIZE;

      validate(vkFlushMappedMemoryRanges(logical_device, 1, &flush_range));
    }

    unmap();
  }

  validate(vkBindBufferMemory(logical_device, _handle, _memory, 0));
}

buffer_base::~buffer_base() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  logical_device.wait_idle();

  vkFreeMemory(logical_device, _memory, nullptr);
  vkDestroyBuffer(logical_device, _handle, nullptr);
}

auto buffer_base::handle() const noexcept -> const VkBuffer& {
  return _handle;
}

buffer_base::operator const VkBuffer&() const noexcept {
  return _handle;
}

auto buffer_base::memory() const noexcept -> const VkDeviceMemory& {
  return _memory;
}

auto buffer_base::size() const noexcept -> std::size_t {
  return _size;
}

auto buffer_base::map() -> memory::observer_ptr<void> {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  auto* mapped_memory = static_cast<void*>(nullptr);

  validate(vkMapMemory(logical_device, _memory, 0, VK_WHOLE_SIZE, 0, &mapped_memory));

  return memory::observer_ptr<void>{mapped_memory};
}

auto buffer_base::unmap() -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  vkUnmapMemory(logical_device, _memory);
}

auto buffer_base::write(memory::observer_ptr<const void> data, size_type size, size_type offset) -> void {
  auto mapped_memory = map();

  std::memcpy(static_cast<std::uint8_t*>(mapped_memory.get()) + offset, data.get(), size);

  unmap();
}

} // namespace sbx::graphics
