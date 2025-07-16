#include <libsbx/graphics/images/image2d.hpp>

#include <stb_image.h>

#include <fmt/format.h>

#include <libsbx/utility/timer.hpp>
#include <libsbx/utility/logger.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>

namespace sbx::graphics {

image2d::image2d(const math::vector2u& extent, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode address_mode, VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
: image{VkExtent3D{extent.x(), extent.y(), 1}, filter, address_mode, samples, layout, (usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), format, 1, 1},
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

image2d::image2d(const math::vector2u& extent, VkFormat format , memory::observer_ptr<const std::uint8_t> pixels)
: image2d{extent, format} {
  set_pixels(pixels);
}

static auto bytes_per_channel(VkFormat format) -> std::uint8_t {
  switch (format) {
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8_SRGB:
    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8_SRGB:
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8_SRGB:
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SRGB:
    case VK_FORMAT_B8G8R8A8_SRGB: {
      return 1;
    }
    case VK_FORMAT_R16_SFLOAT:
    case VK_FORMAT_R16G16_SFLOAT:
    case VK_FORMAT_R16G16B16_SFLOAT:
    case VK_FORMAT_R16G16B16A16_SFLOAT: {
      return 2;
    }
    case VK_FORMAT_R32_SFLOAT:
    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32G32_SFLOAT:
    case VK_FORMAT_R32G32B32A32_SFLOAT: {
      return 4;
    }
    default: {
      throw std::runtime_error{fmt::format("Unsupported image format: {}", static_cast<std::int32_t>(format))};
    }
  }
}

auto image2d::set_pixels(memory::observer_ptr<const std::uint8_t> pixels) -> void {
  auto buffer_size = _extent.width * _extent.height * _channels * bytes_per_channel(_format);
  auto staging_buffer = graphics::staging_buffer{std::span{pixels.get(), buffer_size}};

  transition_image_layout(_handle, _format, _layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);

  copy_buffer_to_image(staging_buffer, _handle, _extent, _array_layers, 0);

  if (_mipmap) {
    create_mipmaps(_handle, _extent, _format, _layout, _mip_levels, 0, _array_layers);
  } else {
    transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _layout, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
  }
}

struct file_header {
  std::uint32_t magic;
  std::uint32_t version;
  std::uint32_t width;
  std::uint32_t height;
  std::uint32_t channels;
}; // struct file_header

struct image_data {
  file_header header;
  std::uint8_t* pixels;
}; // struct image_data

auto write_image(const std::filesystem::path& path, const image_data& data) -> void {
  auto output_file = std::ofstream{path, std::ios::binary};

  if (!output_file.is_open()) {
    throw std::runtime_error{fmt::format("Failed to open image file for writing: {}", path.string())};
  }

  auto header = file_header{};
  header.magic = 69u;
  header.version = 1u;
  header.width = data.header.width;
  header.height = data.header.height;
  header.channels = data.header.channels;

  output_file.write(reinterpret_cast<const char*>(&header), sizeof(file_header));
  output_file.write(reinterpret_cast<const char*>(data.pixels), data.header.width * data.header.height * data.header.channels);

  output_file.close();
}

auto read_image(const std::filesystem::path& path) -> image_data {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error{fmt::format("Image file not found: {}", path.string())};
  }

  auto result = image_data{};

  auto input_file = std::ifstream{path, std::ios::binary};

  if (!input_file.is_open()) {
    throw std::runtime_error{fmt::format("Failed to open image file: {}", path.string())};
  }

  auto header = file_header{};

  input_file.read(reinterpret_cast<char*>(&header), sizeof(file_header));

  if (header.magic != 69u) {
    throw std::runtime_error{fmt::format("Invalid magic number in image file: {}", path.string())};
  }

  if (header.version != 1u) {
    throw std::runtime_error{fmt::format("Unsupported image file version: {} in file: {}", header.version, path.string())};
  }

  result.header = header;
  result.pixels = new std::uint8_t[header.width * header.height * header.channels];
  input_file.read(reinterpret_cast<char*>(result.pixels), header.width * header.height * header.channels);

  input_file.close();

  return result;
}

auto image2d::_load() -> void {
  // [TODO] KAJ 2025-05-26 : This code is absolutely terrible, it should be refactored to use a more robust image loading system.
  // const auto needs_processing = !std::filesystem::exists(std::filesystem::path{_path}.replace_extension(".sbximg"));

  _channels = channels_from_format(_format);

  auto data = image_data{};

  if (!_path.empty()) {
    auto timer = utility::timer{};

    // if (!needs_processing) {
    //   data = read_image(std::filesystem::path{_path}.replace_extension(".sbximg"));
    //   _extent.width = data.header.width;
    //   _extent.height = data.header.height;
    // } else {
    stbi_set_flip_vertically_on_load(true);

    // [NOTE] KAJ 2023-07-28 : Force 4 channels (RGBA) and ignore the original image's channels.
    data.pixels = stbi_load(_path.string().c_str(), reinterpret_cast<std::int32_t*>(&_extent.width), reinterpret_cast<std::int32_t*>(&_extent.height), nullptr, STBI_rgb_alpha);

    if (!data.pixels) {
      throw std::runtime_error{fmt::format("Failed to load image: {}", _path.string())};
    }

    if (_extent.width == 0 || _extent.height == 0) {
      throw std::runtime_error{fmt::format("Image '{}' has invalid dimensions: {}x{}", _path.string(), _extent.width, _extent.height)};
    }

    // data.header.magic = 69u;
    // data.header.version = 1u;
    // data.header.width = _extent.width;
    // data.header.height = _extent.height;
    // data.header.channels = 4u;
    // write_image(std::filesystem::path{_path}.replace_extension(".sbximg"), data);
    // }

    const auto elapsed = units::quantity_cast<units::millisecond>(timer.elapsed());
  
    utility::logger<"graphics">::debug("Loaded image: {} ({}x{}) in {:.2f}ms", _path.string(), _extent.width, _extent.height, elapsed.value());
  }


  _mip_levels = _mipmap ? mip_levels(_extent) : 1;

  create_image(_handle, _allocation, _extent, _format, _samples, VK_IMAGE_TILING_OPTIMAL, _usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _mip_levels, _array_layers, VK_IMAGE_TYPE_2D);
  create_image_sampler(_sampler, _filter, _address_mode, _anisotropic, _mip_levels);
  create_image_view(_handle, _view, VK_IMAGE_VIEW_TYPE_2D, _format, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);

  if (data.pixels || _mipmap) {
    transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
  }

  if (data.pixels) {
    // [NOTE] KAJ 2023-07-28 : Since we loaded the image with STBI_rgb_alpha, we need to multiply the buffer size by 4.
    const auto buffer_size = _extent.width * _extent.height * 4u;
    auto staging_buffer = graphics::staging_buffer{std::span{data.pixels, buffer_size}};

    copy_buffer_to_image(staging_buffer, _handle, _extent, _array_layers, 0);

    // if (needs_processing) {
    stbi_image_free(data.pixels);
    // } else {
    //   delete[] data.pixels;
    // }
  }

  if (_mipmap) {
    create_mipmaps(_handle, _extent, _format, _layout, _mip_levels, 0, _array_layers);
  } else if (data.pixels) {
    transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _layout, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
  } else {
    transition_image_layout(_handle, _format, VK_IMAGE_LAYOUT_UNDEFINED, _layout, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels, 0, _array_layers, 0);
  }
}

} // namespace sbx::graphics

