#include <libsbx/ui/atlas.hpp>

namespace sbx::ui {

atlas::atlas(std::uint32_t width, std::uint32_t height, const std::vector<std::uint8_t>& data)
: _width{width},
  _height{height},
  _image{math::vector2u{width, height}, VK_FORMAT_R8_UNORM} {
  _image.set_pixels(data.data());
}

auto atlas::width() const noexcept -> std::uint32_t {
  return _width;
}

auto atlas::height() const noexcept -> std::uint32_t {
  return _height;
}

auto atlas::image() const noexcept -> const graphics::image2d& {
  return _image;
}

} // namespace sbx::ui
