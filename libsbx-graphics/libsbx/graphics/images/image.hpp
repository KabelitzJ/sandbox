#ifndef LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_
#define LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::graphics {

class image : public utility::noncopyable {

public:

  image(const VkExtent3D& extent, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkFormat format, std::uint32_t mip_levels, std::uint32_t array_layers, VkFilter filter, VkSamplerAddressMode address_mode, VkImageLayout layout);

  ~image();

  auto handle() const noexcept -> const VkImage&;

  operator const VkImage&() const noexcept;

  auto view() const noexcept -> const VkImageView&;

  auto extent() const noexcept -> const VkExtent3D&;

  auto format() const noexcept -> VkFormat;

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
  VkSampler _sampler;
  VkImageView _view;

}; // class image

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_IMAGES_IMAGE_HPP_