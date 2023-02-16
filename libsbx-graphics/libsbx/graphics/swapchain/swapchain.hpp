#ifndef LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_
#define LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_

#include <cinttypes>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include <libsbx/graphics/swapchain/extent.hpp>

namespace sbx::graphics {

class swapchain {

public:

  swapchain(const extent2d& extent, const std::unique_ptr<swapchain>& old_swapchain);

  ~swapchain();

  auto handle() const noexcept -> const VkSwapchainKHR&;

  operator const VkSwapchainKHR&() const noexcept;

  auto extent() const noexcept -> const extent2d&;

  auto active_image_index() const noexcept -> std::uint32_t;

  auto image_count() const noexcept -> std::uint32_t;

  auto pre_transform() const noexcept -> VkSurfaceTransformFlagsKHR;

  auto composite_alpha() const noexcept -> VkCompositeAlphaFlagBitsKHR;

  auto present_mode() const noexcept -> VkPresentModeKHR;

  auto images() const noexcept -> const std::vector<VkImage>&;

  auto image(std::uint32_t index) const noexcept -> const VkImage&;

  auto image_views() const noexcept -> const std::vector<VkImageView>&; 

  auto image_view(std::uint32_t index) const noexcept -> const VkImageView&;

  auto acquire_next_image(const VkSemaphore& image_available_semaphore = nullptr, const VkFence& fence = nullptr) -> VkResult;

  auto queue_present(const VkQueue& queue, const VkSemaphore& wait_semaphore = nullptr) -> VkResult;

private:

  extent2d _extent{};
  VkPresentModeKHR _present_mode{};
  std::uint32_t _active_image_index{};
  std::uint32_t _image_count{};

  VkSurfaceTransformFlagsKHR _pre_transform{};
	VkCompositeAlphaFlagBitsKHR _composite_alpha{};

	std::vector<VkImage> _images{};
	std::vector<VkImageView> _image_views{};
	VkFence _image_fence{};

	VkSwapchainKHR _handle{};


}; // class swapchain

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_
