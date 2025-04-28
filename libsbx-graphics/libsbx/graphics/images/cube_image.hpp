#ifndef LIBSBX_GRAPHICS_IMAGES_CUBE_IMAGE_HPP_
#define LIBSBX_GRAPHICS_IMAGES_CUBE_IMAGE_HPP_

#include <filesystem>

#include <stb_image.h>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/images/image.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>

namespace sbx::graphics {

class cube_image : public image {

public:

  cube_image(const std::filesystem::path& path, const std::string& suffix = ".png", VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, bool anisotropic = true, bool mipmap = true)
  : image{VkExtent3D{0, 0, 1}, filter, address_mode, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), VK_FORMAT_R8G8B8A8_SRGB, 1, 6},
    _anisotropic{anisotropic},
    _mipmap{mipmap},
    _channels{4u},
    _file_path{path},
    _file_suffix{suffix} {
    _load();
  }

  ~cube_image() override = default;

private:

  auto _load() -> void {
    _channels = channels_from_format(_format);

    auto* data = static_cast<std::uint8_t*>(nullptr);

    auto buffer = std::vector<std::uint8_t>{};
    auto offset = std::uint32_t{0};

    if (!_file_path.empty()) {
      auto timer = utility::timer{};

      for (const auto& side : side_names) {
        const auto path = _file_path / fmt::format("{}{}", side, _file_suffix);
        
        auto width = std::int32_t{0};
        auto height = std::int32_t{0};
        auto channels = std::int32_t{0};

        stbi_set_flip_vertically_on_load(true);

        auto* image_data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (!image_data) {
          throw std::runtime_error{fmt::format("Failed to load image: {}", path.string())};
        }

        if (width == 0 || height == 0) {
          throw std::runtime_error{fmt::format("Invalid image size: {} ({}x{})", path.string(), width, height)};
        }

        const auto size = width * height * _channels;

        buffer.resize(buffer.size() + size);
        std::memcpy(buffer.data() + offset, image_data, size);
        offset += size;

        _extent.width = std::max(_extent.width, static_cast<std::uint32_t>(width));
        _extent.height = std::max(_extent.height, static_cast<std::uint32_t>(height));

        stbi_image_free(image_data);
      }

      const auto elapsed = units::quantity_cast<units::millisecond>(timer.elapsed());

      utility::logger<"graphics">::debug("Loaded cube image: {} ({}x{}) in {:.2f}ms", _file_path.string(), _extent.width, _extent.height, elapsed.value());
    }

    if (buffer.empty()) {
      throw std::runtime_error{"Failed to load cube image"};
    }

    data = buffer.data();

    if (_extent.width == 0 || _extent.height == 0) {
      return;
    }

    _mip_levels = _mipmap ? mip_levels(_extent) : 1;

    create_image(_handle, _memory, _extent, _format, _samples, VK_IMAGE_TILING_OPTIMAL, _usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _mip_levels, _array_layers, VK_IMAGE_TYPE_2D);
    create_image_sampler(_sampler, _filter, _address_mode, _anisotropic, _mip_levels);
    create_image_view(_handle, _view, VK_IMAGE_VIEW_TYPE_CUBE, _format, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);

    if (data || _mipmap) {
      transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
    }

    if (data) {
      // [NOTE] KAJ 2023-07-28 : Since we loaded the image with STBI_rgb_alpha, we need to multiply the buffer size by 4.
      auto buffer_size = _extent.width * _extent.height * 4 * _array_layers;
      auto staging_buffer = graphics::staging_buffer{std::span{data, buffer_size}};

      copy_buffer_to_image(staging_buffer, _handle, _extent, _array_layers, 0);
    }

    if (_mipmap) {
      create_mipmaps(_handle, _extent, _format, _layout, _mip_levels, 0, _array_layers);
    } else if (data) {
      transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _layout, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
    } else {
      transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_UNDEFINED, _layout, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
    }
  }

  inline static constexpr auto side_names = std::array<std::string_view, 6u>{"right", "left", "top", "bottom", "front", "back"};

  bool _anisotropic;
  bool _mipmap;
  std::uint8_t _channels;
  std::filesystem::path _file_path;
  std::string _file_suffix;

}; // class cube_image

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_IMAGES_CUBE_IMAGE_HPP_
