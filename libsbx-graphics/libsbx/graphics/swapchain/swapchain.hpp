#ifndef LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_
#define LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_

#include <cinttypes>
#include <memory>

#include <vulkan/vulkan.h>

namespace sbx::graphics {

class swapchain {

public:

  swapchain(const std::unique_ptr<swapchain>& old_swapchain);

  ~swapchain();

  auto active_image() const noexcept -> std::uint32_t;

private:

  std::uint32_t _active_image{};

}; // class swapchain

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_
