#include <libsbx/graphics/swapchain/swapchain.hpp>

namespace sbx::graphics {

swapchain::swapchain(const extent2d& extent, const std::unique_ptr<swapchain>& old_swapchain)
: _extent{extent} {
  static_cast<void>(old_swapchain);
}

swapchain::~swapchain() {

}

auto swapchain::extent() const noexcept -> const extent2d& {
  return _extent;
}

auto swapchain::active_image() const noexcept -> std::uint32_t {
  return _active_image;
}

auto swapchain::image_count() const noexcept -> std::uint32_t {
  return _image_count;
}

} // namespace sbx::graphics
