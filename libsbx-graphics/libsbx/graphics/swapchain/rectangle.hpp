#ifndef LIBSBX_GRAPHICS_SWAPCHAIN_RECTANGLE_HPP_
#define LIBSBX_GRAPHICS_SWAPCHAIN_RECTANGLE_HPP_

#include <cinttypes>

#include <vulkan/vulkan.h>

#include <libsbx/graphics/swapchain/extent.hpp>
#include <libsbx/graphics/swapchain/offset.hpp>

namespace sbx::graphics {

class rectangle2d {

public:

  rectangle2d() = default;

  rectangle2d(const VkRect2D& rectangle);

  rectangle2d(const offset2d& offset, const extent2d& extent);

  ~rectangle2d() = default;

  auto offset() const noexcept -> const offset2d&;

  auto extent() const noexcept -> const extent2d&;

  operator VkRect2D() const noexcept;

private:

  offset2d _offset{};
  extent2d _extent{};

}; // class rectangle2d

auto operator==(const rectangle2d& lhs, const rectangle2d& rhs) noexcept -> bool;

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SWAPCHAIN_RECTANGLE_HPP_
