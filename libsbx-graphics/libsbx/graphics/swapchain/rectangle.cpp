#include <libsbx/graphics/swapchain/rectangle.hpp>

namespace sbx::graphics {

rectangle2d::rectangle2d(const VkRect2D& rectangle)
: _offset{rectangle.offset},
  _extent{rectangle.extent} {}

rectangle2d::rectangle2d(const offset2d& offset, const extent2d& extent)
: _offset{offset},
  _extent{extent} {}

auto rectangle2d::offset() const noexcept -> const offset2d& {
  return _offset;
}

auto rectangle2d::extent() const noexcept -> const extent2d& {
  return _extent;
}

rectangle2d::operator VkRect2D() const noexcept {
  return VkRect2D{_offset, _extent};
}

auto operator==(const rectangle2d& lhs, const rectangle2d& rhs) noexcept -> bool {
  return lhs.offset() == rhs.offset() && lhs.extent() == rhs.extent();
}

} // namespace sbx::graphics
