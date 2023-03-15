#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/core/assert.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::graphics {

buffer::buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
: _size{size},
  _usage{usage},
  _properties{properties} {
  const auto& physical_device = graphics_module::get().physical_device();
  const auto& logical_device = graphics_module::get().logical_device();

  auto buffer_info = VkBufferCreateInfo{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = _size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  validate(vkCreateBuffer(logical_device, &buffer_info, nullptr, &_handle));

  auto memory_requirements = VkMemoryRequirements{};
  vkGetBufferMemoryRequirements(logical_device, _handle, &memory_requirements);

  auto allocation_info = VkMemoryAllocateInfo{};
  allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocation_info.allocationSize = memory_requirements.size;
  allocation_info.memoryTypeIndex = physical_device.find_memory_type(memory_requirements.memoryTypeBits, properties);

  validate(vkAllocateMemory(logical_device, &allocation_info, nullptr, &_memory));

  vkBindBufferMemory(logical_device, _handle, _memory, 0);
}

buffer::buffer(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
: buffer{size, usage, properties} {
  write(data, size);
}

buffer::buffer(const buffer& source, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
: buffer{source.size(), usage, properties} {
  copy_from(source);
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

auto buffer::usage() const noexcept -> VkBufferUsageFlags {
  return _usage;
}

auto buffer::properties() const noexcept -> VkMemoryPropertyFlags {
  return _properties;
}

auto buffer::copy_from(const buffer& src, VkDeviceSize size) const -> void {
  if (size > _size) {
    throw std::runtime_error{"Size is greater than buffer size"};
  } 

  auto command_buffer = graphics::command_buffer{};

  auto copy_region = VkBufferCopy{};
  copy_region.size = size;

  command_buffer.copy_buffer(src, _handle, copy_region);

  command_buffer.submit_idle();
}

auto buffer::copy_from(const buffer& src) const -> void {
  copy_from(src, _size);
}

auto buffer::write(const void* data, VkDeviceSize size, VkDeviceSize offset) const -> void {
  if (size > _size) {
    throw std::runtime_error{"Size is greater than buffer size"};
  } 

  auto* memory = static_cast<std::byte*>(_map());

  std::memcpy(memory + offset, data, size);

  _unmap();
}

auto buffer::_map() const noexcept -> void* {
  const auto& logical_device = graphics_module::get().logical_device();

  auto data = static_cast<void*>(nullptr);

  vkMapMemory(logical_device, _memory, 0, _size, 0, &data);

  return data;
}

auto buffer::_unmap() const noexcept -> void {
  const auto& logical_device = graphics_module::get().logical_device();

  vkUnmapMemory(logical_device, _memory);
}

} // namespace sbx::graphics
