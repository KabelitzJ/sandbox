#ifndef LIBSBX_GRAPHICS_SWAPCHAIN_EXTENT_HPP_
#define LIBSBX_GRAPHICS_SWAPCHAIN_EXTENT_HPP_

#include <cinttypes>

#include <vulkan/vulkan.h>

namespace sbx::graphics {

class extent2d {

public:

  extent2d() = default;

  extent2d(const VkExtent2D& extent);

  extent2d(std::uint32_t width, std::uint32_t height);

  ~extent2d() = default;

  auto width() const noexcept -> std::uint32_t;

  auto height() const noexcept -> std::uint32_t;

  operator VkExtent2D() const noexcept;

private:

  std::uint32_t _width{};
  std::uint32_t _height{};

}; // class extent2d

auto operator==(const extent2d& lhs, const extent2d& rhs) noexcept -> bool;

class extent3d {

public:

  extent3d() = default;

  extent3d(const VkExtent3D& extent);

  extent3d(std::uint32_t width, std::uint32_t height, std::uint32_t depth);

  ~extent3d() = default;

  auto width() const noexcept -> std::uint32_t;

  auto height() const noexcept -> std::uint32_t;

  auto depth() const noexcept -> std::uint32_t;

  operator VkExtent3D() const noexcept;

private:

  std::uint32_t _width{};
  std::uint32_t _height{};
  std::uint32_t _depth{};

}; // class extent3d

auto operator==(const extent3d& lhs, const extent3d& rhs) noexcept -> bool;

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SWAPCHAIN_EXTENT_HPP_
