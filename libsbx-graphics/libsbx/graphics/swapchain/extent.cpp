#include <libsbx/graphics/swapchain/extent.hpp>

namespace sbx::graphics {

extent2d::extent2d(const VkExtent2D& extent)
: _width{extent.width},
  _height{extent.height} {}

extent2d::extent2d(std::uint32_t width, std::uint32_t height) 
: _width{width}, 
_height{height} {}

auto extent2d::width() const noexcept -> std::uint32_t {
  return _width;
}

auto extent2d::height() const noexcept -> std::uint32_t {
  return _height;
}

extent2d::operator VkExtent2D() const noexcept {
  return VkExtent2D{_width, _height};
}

auto operator==(const extent2d& lhs, const extent2d& rhs) noexcept -> bool {
  return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

extent3d::extent3d(const VkExtent3D& extent)
: _width{extent.width},
  _height{extent.height},
  _depth{extent.depth} {}

extent3d::extent3d(std::uint32_t width, std::uint32_t height, std::uint32_t depth)
: _width{width},
  _height{height},
  _depth{depth} {}

auto extent3d::width() const noexcept -> std::uint32_t {
  return _width;
}

auto extent3d::height() const noexcept -> std::uint32_t {
  return _height;
}

auto extent3d::depth() const noexcept -> std::uint32_t {
  return _depth;
}

extent3d::operator VkExtent3D() const noexcept {
  return VkExtent3D{_width, _height, _depth};
}

auto operator==(const extent3d& lhs, const extent3d& rhs) noexcept -> bool {
  return lhs.width() == rhs.width() && lhs.height() == rhs.height() && lhs.depth() == rhs.depth();
}

} // namespace sbx::graphics
