#ifndef LIBSBX_GRAPHICS_IMAGES_IMAGE2D_HPP_
#define LIBSBX_GRAPHICS_IMAGES_IMAGE2D_HPP_

#include <filesystem>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/math/vector2.hpp>

#include <libsbx/graphics/resource_storage.hpp>

#include <libsbx/graphics/images/image.hpp>

namespace sbx::graphics {

class image2d : public image {

public:

  image2d(const math::vector2u& extent, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, bool anisotropic = false, bool mipmap = false);

  image2d(const std::filesystem::path& path, VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT, bool anisotropic = false, bool mipmap = false);

  image2d(const math::vector2u& extent, VkFormat format, memory::observer_ptr<const std::uint8_t> pixels);

  ~image2d() override = default;

  auto set_pixels(memory::observer_ptr<const std::uint8_t> pixels) -> void;

private:

  auto _load() -> void;

  bool _anisotropic;
	bool _mipmap;
  std::uint8_t _channels;
  std::filesystem::path _path;

}; // class image2d

using image2d_handle = resource_handle<image2d>;

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_IMAGES_IMAGE2D_HPP_
