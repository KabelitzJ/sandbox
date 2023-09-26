#include <libsbx/graphics/images/image2d.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fmt/format.h>

#include <libsbx/utility/timer.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

namespace sbx::graphics {

image2d::image2d(const math::vector2u& extent, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode address_mode, VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
: image{VkExtent3D{extent.x, extent.y, 1}, filter, address_mode, samples, layout, (usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), format, 1, 1},
  _anisotropic{anisotropic},
  _mipmap{mipmap} {
  _load();
}

// [TODO] KAJ 2023-07-28 : We use VK_FORMAT_R8G8B8A8_SRGB here because it best matches the STBI_rgb_alpha format that we load the image with.
image2d::image2d(const std::filesystem::path& path, VkFilter filter, VkSamplerAddressMode address_mode, bool anisotropic, bool mipmap)
: image{VkExtent3D{0, 0, 1}, filter, address_mode, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), VK_FORMAT_R8G8B8A8_SRGB, 1, 1},
  _anisotropic{anisotropic},
  _mipmap{mipmap},
  _path{path} {
  _load();
}

image2d::~image2d() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();
  
  logical_device.wait_idle();
}

auto image2d::set_pixels(memory::observer_ptr<const std::uint8_t> pixels) -> void {
  auto buffer_size = _extent.width * _extent.height * _channels;
  auto staging_buffer = graphics::buffer{buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pixels.get()};

  transition_image_layout(_handle, _format, _layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);

  copy_buffer_to_image(staging_buffer, _handle, _extent, _array_layers, 0);

  if (_mipmap) {
    create_mipmaps(_handle, _extent, _format, _layout, _mip_levels, 0, _array_layers);
  } else {
    transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _layout, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
  }
}

auto image2d::_load() -> void {
  switch (_format) {
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8_SRGB: {
      _channels = 1;
      break;
    }
    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8_SRGB: {
      _channels = 2;
      break;
    }
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8_SRGB: {
      _channels = 3;
      break;
    }
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SRGB: {
      _channels = 4;
      break;
    }
    default: {
      throw std::runtime_error{fmt::format("Unsupported image format: {}", static_cast<std::int32_t>(_format))};
    }
  }

  auto* data = static_cast<std::uint8_t*>(nullptr);

  if (!_path.empty()) {
    auto timer = utility::timer{};

    stbi_set_flip_vertically_on_load(true);

    // [NOTE] KAJ 2023-07-28 : Force 4 channels (RGBA) and ignore the original image's channels.
    data = stbi_load(_path.string().c_str(), reinterpret_cast<std::int32_t*>(&_extent.width), reinterpret_cast<std::int32_t*>(&_extent.height), nullptr, STBI_rgb_alpha);

    core::logger::debug("Loaded image: {} ({}x{}) in {}ms", _path.string(), _extent.width, _extent.height, units::quantity_cast<units::millisecond>(timer.elapsed()).value());

    if (!data) {
      throw std::runtime_error{fmt::format("Failed to load image: {}", _path.string())};
    }
  }

  if (_extent.width == 0 || _extent.height == 0) {
    return;
  }

  _mip_levels = _mipmap ? mip_levels(_extent) : 1;

  create_image(_handle, _memory, _extent, _format, _samples, VK_IMAGE_TILING_OPTIMAL, _usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _mip_levels, _array_layers, VK_IMAGE_TYPE_2D);
  create_image_sampler(_sampler, _filter, _address_mode, _anisotropic, _mip_levels);
  create_image_view(_handle, _view, VK_IMAGE_VIEW_TYPE_2D, _format, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);

  if (data || _mipmap) {
    transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
  }

  if (data) {
    // [NOTE] KAJ 2023-07-28 : Since we loaded the image with STBI_rgb_alpha, we need to multiply the buffer size by 4.
    auto buffer_size = _extent.width * _extent.height * 4;
    auto staging_buffer = buffer{buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, data};

    copy_buffer_to_image(staging_buffer, _handle, _extent, _array_layers, 0);

    stbi_image_free(data);
  }

  if (_mipmap) {
    create_mipmaps(_handle, _extent, _format, _layout, _mip_levels, 0, _array_layers);
  } else if (data) {
    transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _layout, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
  } else {
    transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_UNDEFINED, _layout, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
  }
}

} // namespace sbx::graphics

