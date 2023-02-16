#ifndef LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_
#define LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_

#include <cinttypes>
#include <memory>

#include <vulkan/vulkan.h>

#include <libsbx/graphics/swapchain/extent.hpp>

namespace sbx::graphics {

class swapchain {

public:

  swapchain(const extent2d& extent, const std::unique_ptr<swapchain>& old_swapchain);

  ~swapchain();

  auto extent() const noexcept -> const extent2d&;

  auto active_image() const noexcept -> std::uint32_t;

  auto image_count() const noexcept -> std::uint32_t;

private:

  extent2d _extent{};

  std::uint32_t _active_image{};
  std::uint32_t _image_count{};

}; // class swapchain

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_
