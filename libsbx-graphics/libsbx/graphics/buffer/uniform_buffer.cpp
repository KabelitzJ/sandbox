#include <libsbox/graphics/buffer/uniform_buffer.hpp>

namespace sbx::graphics {

uniform_buffer::uniform_buffer(VkDeviceSize size, const void* data)
: buffer{data, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT} { }

auto uniform_buffer::update(const void* data) -> void {
  write(data, _size);
}

auto uniform_buffer::write_descriptor_set(std::uint32_t binding, VkDescriptorType descriptor_type) const noexcept -> graphics::write_descriptor_set {
  auto buffer_info = VkDescriptorBufferInfo{};
  buffer_info.buffer = _handle;
  buffer_info.offset = 0;
  buffer_info.range = _size;

  auto write_descriptor_set = VkWriteDescriptorSet{};
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.dstSet = nullptr;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.descriptorType = descriptor_type;

  return write_descriptor_set{write_descriptor_set, buffer_info};
}

} // namespace sbx::graphics
