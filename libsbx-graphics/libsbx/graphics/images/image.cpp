#include <libsbx/graphics/images/image.hpp>

#include <cmath>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

static const auto depth_formats = std::vector<VkFormat>{
  VK_FORMAT_D16_UNORM, 
  VK_FORMAT_X8_D24_UNORM_PACK32, 
  VK_FORMAT_D32_SFLOAT, 
  VK_FORMAT_D16_UNORM_S8_UINT,
  VK_FORMAT_D24_UNORM_S8_UINT,
  VK_FORMAT_D32_SFLOAT_S8_UINT
};

static const auto stencil_formats = std::vector<VkFormat>{
  VK_FORMAT_S8_UINT, 
  VK_FORMAT_D16_UNORM_S8_UINT, 
  VK_FORMAT_D24_UNORM_S8_UINT, 
  VK_FORMAT_D32_SFLOAT_S8_UINT
};

image::image(const VkExtent3D extent, VkFilter filter, VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, VkImageLayout layout, VkImageUsageFlags usage, VkFormat format, std::uint32_t mipLevels, std::uint32_t arrayLayers)
: _extent{extent},
  _filter{filter},
  _address_mode{addressMode},
  _samples{samples},
  _layout{layout},
  _usage{usage},
  _format{format},
  _mip_levels{mipLevels},
  _array_layers{arrayLayers} { }

image::~image() {
  auto& logical_device = graphics_module::get().logical_device();

  vkDestroyImageView(logical_device, _view, nullptr);
  vkDestroySampler(logical_device, _sampler, nullptr);
  vkFreeMemory(logical_device, _memory, nullptr);
  vkDestroyImage(logical_device, _handle, nullptr);
}

auto image::descriptor_set_layout(std::uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags shader_stage_flags, std::uint32_t count) noexcept -> VkDescriptorSetLayoutBinding {
  auto descriptor_set_layout_binding = VkDescriptorSetLayoutBinding{};
  descriptor_set_layout_binding.binding = binding;
  descriptor_set_layout_binding.descriptorType = descriptor_type;
  descriptor_set_layout_binding.stageFlags = shader_stage_flags;
  descriptor_set_layout_binding.descriptorCount = count;
  descriptor_set_layout_binding.pImmutableSamplers = nullptr;

  return descriptor_set_layout_binding;
}

auto image::mip_levels(const VkExtent3D& extent) noexcept -> std::uint32_t {
  return static_cast<std::uint32_t>(std::floor(std::log2(std::max(extent.width, std::max(extent.height, extent.depth)))) + 1);
}

auto image::find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) noexcept -> VkFormat {
  auto& physical_device = graphics_module::get().physical_device();

  for (const auto& format : candidates) {
    auto format_properties = VkFormatProperties{};
    vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);

    if (tiling == VK_IMAGE_TILING_LINEAR && (format_properties.linearTilingFeatures & features) == features) {
      return format;
    } 
    
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (format_properties.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  return VK_FORMAT_UNDEFINED;
}

auto image::has_depth_component(VkFormat format) noexcept -> bool {
  return std::find(depth_formats.begin(), depth_formats.end(), format) != depth_formats.end();
}

auto image::has_stencil_component(VkFormat format) noexcept -> bool {
  return std::find(stencil_formats.begin(), stencil_formats.end(), format) != stencil_formats.end();
}

auto image::create_image(VkImage& image, VkDeviceMemory& memory, const VkExtent3D& extent, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkFormat format, std::uint32_t mip_levels, std::uint32_t array_layers, VkImageLayout layout) -> void {
  auto& logical_device = graphics_module::get().logical_device();

  auto image_create_info = VkImageCreateInfo{};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.flags = array_layers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
  image_create_info.imageType = type;
  image_create_info.format = format;
  image_create_info.extent = extent;
  image_create_info.mipLevels = mip_levels;
  image_create_info.arrayLayers = array_layers;
  image_create_info.samples = samples;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_create_info.usage = usage;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_create_info.initialLayout = layout;

  validate(vkCreateImage(logical_device, &image_create_info, nullptr, &image));
}

auto image::create_image_view(const VkImage& image, VkImageView& image_view, VkImageViewType type, VkFormat format, VkImageAspectFlags image_aspect, std::uint32_t mip_levels, std::uint32_t base_mip_level, std::uint32_t layer_count, std::uint32_t base_array_layer) -> void {

}

auto image::create_sampler(VkSampler& sampler, VkFilter filter, VkSamplerAddressMode address_mode, bool anisotropic, std::uint32_t mip_levels) -> void {

}

auto image::create_mipmaps(const VkImage& image, const VkExtent3D& extent, VkFormat format, VkImageLayout dst_image_layout, std::uint32_t mip_levels, std::uint32_t base_array_layer, std::uint32_t layer_count) -> void {

}

auto image::transition_image_layout(const VkImage& image, VkFormat format, VkImageLayout src_image_layout, VkImageLayout dst_image_layout, VkImageAspectFlags image_aspect, std::uint32_t mip_levels, std::uint32_t base_mip_level, std::uint32_t layer_count, std::uint32_t base_array_layer) -> void {

}

auto image::copy_buffer_to_image(const VkBuffer& buffer, const VkImage& image, const VkExtent3D& extent, std::uint32_t layer_count, std::uint32_t base_array_layer) -> void {

}

auto image::copy_image(const VkImage& src_image, VkImage& dst_image, VkDeviceMemory& dst_image_memory, VkFormat src_format, const VkExtent3D& extent, VkImageLayout src_image_layout, std::uint32_t mip_level, std::uint32_t array_layer) -> bool {

}

auto image::extent() const noexcept -> const VkExtent3D& {

}

auto image::size() const noexcept -> math::vector2u {

}

auto image::format() const noexcept -> VkFormat {

}

auto image::samples() const noexcept -> VkSampleCountFlagBits {

}

auto image::usage() const noexcept -> VkImageUsageFlags {

}

auto image::mip_levels() const noexcept -> std::uint32_t {

}

auto image::array_layers() const noexcept -> std::uint32_t {

}

auto image::filter() const noexcept -> VkFilter {

}

auto image::address_mode() const noexcept -> VkSamplerAddressMode {

}

auto image::layout() const noexcept -> VkImageLayout {

}

auto image::handle() const noexcept -> const VkImage& {

}

image::operator const VkImage&() const noexcept {

}

auto image::view() const noexcept -> const VkImageView& {

}

auto image::memory() const noexcept -> const VkDeviceMemory& {

}

auto image::sampler() const noexcept -> const VkSampler& {

}

} // namespace sbx::graphics
