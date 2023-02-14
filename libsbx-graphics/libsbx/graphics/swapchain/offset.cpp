#include <libsbx/graphics/swapchain/offset.hpp>

namespace sbx::graphics {

offset2d::offset2d(std::int32_t x, std::int32_t y)
: _x{x},
  _y{y} {}

auto offset2d::x() const noexcept -> std::int32_t {
  return _x;
}

auto offset2d::y() const noexcept -> std::int32_t {
  return _y;
}

offset2d::operator VkOffset2D() const noexcept {
  return VkOffset2D{_x, _y};
}

auto operator==(const offset2d& lhs, const offset2d& rhs) noexcept -> bool {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

offset3d::offset3d(std::int32_t x, std::int32_t y, std::int32_t z)
: _x{x},
  _y{y},
  _z{z} {}

auto offset3d::x() const noexcept -> std::int32_t {
  return _x;
}

auto offset3d::y() const noexcept -> std::int32_t {
  return _y;
}

auto offset3d::z() const noexcept -> std::int32_t {
  return _z;
}

offset3d::operator VkOffset3D() const noexcept {
  return VkOffset3D{_x, _y, _z};
}

auto operator==(const offset3d& lhs, const offset3d& rhs) noexcept -> bool {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
}

} // namespace sbx::graphics
