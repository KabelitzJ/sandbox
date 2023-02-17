#ifndef LIBSBX_GRAPHICS_SWAPCHAIN_OFFSET_HPP_
#define LIBSBX_GRAPHICS_SWAPCHAIN_OFFSET_HPP_

#include <cinttypes>

#include <vulkan/vulkan.h>

namespace sbx::graphics {

class offset2d {

public:

  offset2d() = default;

  offset2d(const VkOffset2D& offset);

  offset2d(std::int32_t x, std::int32_t y);

  ~offset2d() = default;

  auto x() const noexcept -> std::int32_t;

  auto y() const noexcept -> std::int32_t;

  operator VkOffset2D() const noexcept;

private:

  std::int32_t _x{};
  std::int32_t _y{};

}; // class offset2d

auto operator==(const offset2d& lhs, const offset2d& rhs) noexcept -> bool;

class offset3d {

public:

  offset3d() = default;

  offset3d(const VkOffset3D& offset);

  offset3d(std::int32_t x, std::int32_t y, std::int32_t z);

  ~offset3d() = default;

  auto x() const noexcept -> std::int32_t;

  auto y() const noexcept -> std::int32_t;

  auto z() const noexcept -> std::int32_t;

  operator VkOffset3D() const noexcept;

private:

  std::int32_t _x{};
  std::int32_t _y{};
  std::int32_t _z{};

}; // class offset3d

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SWAPCHAIN_OFFSET_HPP_
