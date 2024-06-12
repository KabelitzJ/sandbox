#ifndef LIBSBX_GRAPHICS_IMAGES_SEPARATE_IMAGE2D_ARRAY_HPP_
#define LIBSBX_GRAPHICS_IMAGES_SEPARATE_IMAGE2D_ARRAY_HPP_

#include <vector>
#include <unordered_map>

#include <libsbx/math/uuid.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/descriptor/descriptor.hpp>

namespace sbx::graphics {

class separate_image2d_array : public descriptor {

public:

  inline static constexpr auto max_size = std::uint32_t{32u};

  separate_image2d_array() {

  }

  ~separate_image2d_array() {

  }

  static auto create_descriptor_set_layout_binding(std::uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags shader_stage_flags) noexcept -> VkDescriptorSetLayoutBinding {
    auto descriptor_set_layout_binding = VkDescriptorSetLayoutBinding{};
    descriptor_set_layout_binding.binding = binding;
    descriptor_set_layout_binding.descriptorType = descriptor_type;
    descriptor_set_layout_binding.stageFlags = shader_stage_flags;
    descriptor_set_layout_binding.descriptorCount = max_size;
    descriptor_set_layout_binding.pImmutableSamplers = nullptr;

    return descriptor_set_layout_binding;
  }

  auto write_descriptor_set(std::uint32_t binding, VkDescriptorType descriptor_type) const noexcept -> graphics::write_descriptor_set override {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
      
    auto descriptor_image_infos = std::vector<VkDescriptorImageInfo>{};

    for (const auto id : _image_ids) {
      auto& image = graphics_module.get_asset<graphics::image2d>(id);

      auto descriptor_image_info = VkDescriptorImageInfo{};
      descriptor_image_info.imageLayout = image.layout();
      descriptor_image_info.imageView = image.view();
      
      descriptor_image_infos.push_back(descriptor_image_info);
    }

    auto descriptor_write = VkWriteDescriptorSet{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = nullptr;
    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorCount = static_cast<std::uint32_t>(descriptor_image_infos.size());
    descriptor_write.descriptorType = descriptor_type;

    return graphics::write_descriptor_set{descriptor_write, descriptor_image_infos};
  }

  auto push_back(const math::uuid& id) -> std::uint32_t {
    if (_image_ids.size() > max_size) {
      throw std::runtime_error{"separate_image2d_array::push_back: max_size exceeded"};
    }

    if (const auto entry = _id_to_indices.find(id); entry != _id_to_indices.cend()) {
      return entry->second;
    }

    const auto index = static_cast<std::uint32_t>(_image_ids.size());

    _image_ids.push_back(id);
    _id_to_indices.insert({id, index});

    return index;
  }

  auto clear() -> void {
    _image_ids.clear();
    _id_to_indices.clear();
  }

private:

  std::vector<math::uuid> _image_ids;
  std::unordered_map<math::uuid, std::uint32_t> _id_to_indices;

}; // class separate_image2d_array

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_IMAGES_SEPARATE_IMAGE2D_ARRAY_HPP_
