#include <libsbx/graphics/images/image.hpp>

namespace sbx::graphics {

image::image(const VkExtent3D& extent, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkFormat format, std::uint32_t mip_levels, std::uint32_t array_layers, VkFilter filter, VkSamplerAddressMode address_mode, VkImageLayout layout) {

}

image::~image() {

}

auto image::handle() const noexcept -> const VkImage& {
  return _handle;
}

image::operator const VkImage&() const noexcept {
  return _handle;
}

auto image::view() const noexcept -> const VkImageView& {
  return _view;
}

auto image::extent() const noexcept -> const VkExtent3D& {
  return _extent;
}

auto image::format() const noexcept -> VkFormat {
  return _format;
}

} // namespace sbx::graphics
