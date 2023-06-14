#ifndef LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_
#define LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_

#include <cinttypes>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/math/vector2.hpp>

namespace sbx::graphics {

class image : public utility::noncopyable {

public:

  image(const VkExtent3D extent, VkFilter filter, VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, VkImageLayout layout, VkImageUsageFlags usage, VkFormat format, std::uint32_t mipLevels, std::uint32_t arrayLayers);

  ~image();

  static auto descriptor_set_layout(std::uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags shader_stage_flags, std::uint32_t count = 1u) noexcept -> VkDescriptorSetLayoutBinding;

  static auto mip_levels(const VkExtent3D& extent) noexcept -> std::uint32_t;

  static auto find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) noexcept -> VkFormat;

  static auto has_depth_component(VkFormat format) noexcept -> bool;

  static auto has_stencil_component(VkFormat format) noexcept -> bool;

  static auto create_image(VkImage& image, VkDeviceMemory& memory, const VkExtent3D& extent, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkFormat format, std::uint32_t mip_levels, std::uint32_t array_layers, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED) -> void;

  static auto create_image_view(const VkImage& image, VkImageView& image_view, VkImageViewType type, VkFormat format, VkImageAspectFlags image_aspect, std::uint32_t mip_levels, std::uint32_t base_mip_level, std::uint32_t layer_count, std::uint32_t base_array_layer) -> void;

  static auto create_sampler(VkSampler& sampler, VkFilter filter, VkSamplerAddressMode address_mode, bool anisotropic, std::uint32_t mip_levels) -> void;

  static auto create_mipmaps(const VkImage& image, const VkExtent3D& extent, VkFormat format, VkImageLayout dst_image_layout, std::uint32_t mip_levels, std::uint32_t base_array_layer, std::uint32_t layer_count) -> void;

  static auto transition_image_layout(const VkImage& image, VkFormat format, VkImageLayout src_image_layout, VkImageLayout dst_image_layout, VkImageAspectFlags image_aspect, std::uint32_t mip_levels, std::uint32_t base_mip_level, std::uint32_t layer_count, std::uint32_t base_array_layer) -> void;

  static auto copy_buffer_to_image(const VkBuffer& buffer, const VkImage& image, const VkExtent3D& extent, std::uint32_t layer_count, std::uint32_t base_array_layer) -> void;

	static auto copy_image(const VkImage& src_image, VkImage& dst_image, VkDeviceMemory& dst_image_memory, VkFormat src_format, const VkExtent3D& extent, VkImageLayout src_image_layout, std::uint32_t mip_level, std::uint32_t array_layer) -> bool;

  auto extent() const noexcept -> const VkExtent3D&;

  auto size() const noexcept -> math::vector2u;

  auto format() const noexcept -> VkFormat;

  auto samples() const noexcept -> VkSampleCountFlagBits;

  auto usage() const noexcept -> VkImageUsageFlags;

  auto mip_levels() const noexcept -> std::uint32_t;

  auto array_layers() const noexcept -> std::uint32_t;

  auto filter() const noexcept -> VkFilter;

  auto address_mode() const noexcept -> VkSamplerAddressMode;

  auto layout() const noexcept -> VkImageLayout;

  auto handle() const noexcept -> const VkImage&;

  operator const VkImage&() const noexcept;

  auto view() const noexcept -> const VkImageView&;

  auto memory() const noexcept -> const VkDeviceMemory&;

  auto sampler() const noexcept -> const VkSampler&;

private:

  VkExtent3D _extent;
  VkSampleCountFlagBits _samples;
  VkImageUsageFlags _usage;
  VkFormat _format;
  std::uint32_t _mip_levels;
  std::uint32_t _array_layers;

  VkFilter _filter;
  VkSamplerAddressMode _address_mode;

  VkImageLayout _layout;

  VkImage _handle;
  VkDeviceMemory _memory;
  VkImageView _view;
  VkSampler _sampler;

}; // class image

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_
