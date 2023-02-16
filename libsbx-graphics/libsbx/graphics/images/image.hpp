#ifndef LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_
#define LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

class image {

public:

  static auto create_image_view(const VkImage& image, VkImageView& image_view, VkImageViewType type, VkFormat format, VkImageAspectFlags image_aspect, std::uint32_t mip_levels, std::uint32_t base_mip_level, std::uint32_t layer_count, std::uint32_t base_array_layer) -> void {
    auto& logical_device = graphics_module::get().logical_device();

    auto image_view_create_info = VkImageViewCreateInfo{};

    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = image;
    image_view_create_info.viewType = type;
    image_view_create_info.format = format;
    image_view_create_info.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    image_view_create_info.subresourceRange.aspectMask = image_aspect;
    image_view_create_info.subresourceRange.baseMipLevel = base_mip_level;
    image_view_create_info.subresourceRange.levelCount = mip_levels;
    image_view_create_info.subresourceRange.baseArrayLayer = base_array_layer;
    image_view_create_info.subresourceRange.layerCount = layer_count;

    validate(vkCreateImageView(logical_device, &image_view_create_info, nullptr, &image_view));
  }

}; // class image

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_
