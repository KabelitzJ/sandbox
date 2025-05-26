#include <libsbx/graphics/images/separate_sampler.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/images/image.hpp>

namespace sbx::graphics {

separate_sampler::separate_sampler(VkFilter filter, VkSamplerAddressMode address_mode) {
  image::create_image_sampler(_handle, filter, address_mode, false, 1u);
}

separate_sampler::~separate_sampler() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
  auto& logical_device = graphics_module.logical_device();

  vkDestroySampler(logical_device, _handle, nullptr);
}

auto separate_sampler::create_descriptor_set_layout_binding(std::uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags shader_stage_flags) noexcept -> VkDescriptorSetLayoutBinding {
  auto descriptor_set_layout_binding = VkDescriptorSetLayoutBinding{};
  descriptor_set_layout_binding.binding = binding;
  descriptor_set_layout_binding.descriptorType = descriptor_type;
  descriptor_set_layout_binding.stageFlags = shader_stage_flags;
  descriptor_set_layout_binding.descriptorCount = 1u;
  descriptor_set_layout_binding.pImmutableSamplers = nullptr;

  return descriptor_set_layout_binding;
}

auto separate_sampler::write_descriptor_set(std::uint32_t binding, VkDescriptorType descriptor_type) const noexcept -> graphics::write_descriptor_set {
  auto descriptor_image_infos = std::vector<VkDescriptorImageInfo>{};

  auto descriptor_image_info = VkDescriptorImageInfo{};
  descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  descriptor_image_info.imageView = nullptr;
  descriptor_image_info.sampler = _handle;

  descriptor_image_infos.push_back(descriptor_image_info);

  auto descriptor_write = VkWriteDescriptorSet{};
  descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptor_write.dstSet = nullptr;
  descriptor_write.dstBinding = binding;
  descriptor_write.dstArrayElement = 0;
  descriptor_write.descriptorCount = 1;
  descriptor_write.descriptorType = descriptor_type;

  return graphics::write_descriptor_set{descriptor_write, descriptor_image_infos};
}

auto separate_sampler::handle() const noexcept -> VkSampler {
  return _handle;
}

separate_sampler::operator VkSampler() const noexcept {
  return _handle;
}

} // namespace sbx::graphics
