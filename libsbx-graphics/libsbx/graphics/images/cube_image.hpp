#ifndef LIBSBX_GRAPHICS_IMAGES_CUBE_IMAGE_HPP_
#define LIBSBX_GRAPHICS_IMAGES_CUBE_IMAGE_HPP_

#include <filesystem>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/images/image.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>

#include <libsbx/graphics/resource_storage.hpp>

namespace sbx::graphics {

class cube_image : public image {

public:

  cube_image(const std::filesystem::path& path, const std::string& suffix = ".png", VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, bool anisotropic = true, bool mipmap = true);

  ~cube_image() override;

private:

  auto _load() -> void;

  inline static constexpr auto side_names = std::array<std::string_view, 6u>{"right", "left", "top", "bottom", "front", "back"};

  bool _anisotropic;
  bool _mipmap;
  std::uint8_t _channels;
  std::filesystem::path _file_path;
  std::string _file_suffix;

}; // class cube_image

using cube_image_handle = resource_handle<cube_image>;

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_IMAGES_CUBE_IMAGE_HPP_
