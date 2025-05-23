#include <libsbx/graphics/buffers/buffer.hpp>

#include <fmt/format.h>

#include <libsbx/utility/assert.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::graphics {

buffer_base::buffer_base(size_type size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, memory::observer_ptr<const void> memory)
: _size{size},
  _usage{usage} {
  utility::assert_that(size > 0, "Buffer size must be greater than 0.");

  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto allocator = graphics_module.allocator();

  auto buffer_create_info = VkBufferCreateInfo{};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = _size;
  buffer_create_info.usage = _usage;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  auto allocation_create_info = VmaAllocationCreateInfo{};
  allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;
  allocation_create_info.requiredFlags = properties;

  if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
    allocation_create_info.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    allocation_create_info.requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    allocation_create_info.preferredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
  }

  if (properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
    allocation_create_info.requiredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  }

  validate(vmaCreateBuffer(allocator, &buffer_create_info, &allocation_create_info, &_handle, &_allocation, nullptr));

  if (memory) {
    auto* mapped_memory = static_cast<void*>(nullptr);
    validate(vmaMapMemory(allocator, _allocation, &mapped_memory));
    std::memcpy(mapped_memory, memory.get(), size);
    vmaUnmapMemory(allocator, _allocation);
  }

  if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
    auto buffer_device_address_info = VkBufferDeviceAddressInfo{};
    buffer_device_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    buffer_device_address_info.buffer = _handle;
    _address = vkGetBufferDeviceAddress(graphics_module.logical_device(), &buffer_device_address_info);
  } else {
    _address = 0u;
  }
}

buffer_base::~buffer_base() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto allocator = graphics_module.allocator();

  const auto& logical_device = graphics_module.logical_device();

  logical_device.wait_idle();

  vmaDestroyBuffer(allocator, _handle, _allocation);
}

auto buffer_base::handle() const noexcept -> const VkBuffer& {
  return _handle;
}

buffer_base::operator const VkBuffer&() const noexcept {
  return _handle;
}

auto buffer_base::address() const noexcept -> std::uint64_t {
  utility::assert_that((_usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT), "Attempting to get address of buffer that was not created with VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set");
  return _address;
}

auto buffer_base::size() const noexcept -> std::size_t {
  return _size;
}

auto buffer_base::map() -> memory::observer_ptr<void> {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto allocator = graphics_module.allocator();

  auto* mapped_memory = static_cast<void*>(nullptr);

  validate(vmaMapMemory(allocator, _allocation, &mapped_memory));

  return memory::observer_ptr<void>{mapped_memory};
}

auto buffer_base::unmap() -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto allocator = graphics_module.allocator();

  const auto& logical_device = graphics_module.logical_device();

  vmaUnmapMemory(allocator, _allocation);
}

auto buffer_base::write(memory::observer_ptr<const void> data, size_type size, size_type offset) -> void {
  auto mapped_memory = map();

  std::memcpy(static_cast<std::uint8_t*>(mapped_memory.get()) + offset, data.get(), size);

  unmap();
}

} // namespace sbx::graphics
