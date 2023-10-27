#include <libsbx/graphics/buffer/storage_buffer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

storage_buffer::storage_buffer(VkDeviceSize size, memory::observer_ptr<void> data)
: buffer{size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, data}, 
  _mapped_memory{map()} { }

storage_buffer::~storage_buffer() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  logical_device.wait_idle();
  
  unmap();
}

auto storage_buffer::update(memory::observer_ptr<void> data) -> void {
  std::memcpy(static_cast<std::uint8_t*>(_mapped_memory.get()), data.get(), size());
}

auto storage_buffer::write_descriptor_set(std::uint32_t binding, VkDescriptorType descriptor_type) const noexcept -> graphics::write_descriptor_set {
  auto buffer_info = VkDescriptorBufferInfo{};
  buffer_info.buffer = handle();
  buffer_info.offset = 0;
  buffer_info.range = size();

  auto write_descriptor_set = VkWriteDescriptorSet{};
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.dstSet = nullptr;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.descriptorType = descriptor_type;

  return graphics::write_descriptor_set{write_descriptor_set, buffer_info};
}

auto storage_buffer::create_descriptor_set_layout_binding(std::uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags) noexcept -> VkDescriptorSetLayoutBinding {
  auto descriptor_set_layout_binding = VkDescriptorSetLayoutBinding{};
  descriptor_set_layout_binding.binding = binding;
  descriptor_set_layout_binding.descriptorType = descriptor_type;
  descriptor_set_layout_binding.descriptorCount = 1;
  descriptor_set_layout_binding.stageFlags = stage_flags;
  descriptor_set_layout_binding.pImmutableSamplers = nullptr;

  return descriptor_set_layout_binding;
}

} // namespace sbx::graphics
