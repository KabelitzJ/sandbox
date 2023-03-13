#ifndef LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_
#define LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_

#include <cinttypes>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include <libsbx/graphics/render_pass/render_pass.hpp>

#include <libsbx/graphics/devices/physical_device.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>
#include <libsbx/graphics/devices/surface.hpp>

namespace sbx::graphics {

class swapchain {

public:

  swapchain(const physical_device& physical_device, const logical_device& logical_device, const surface& surface, const VkExtent2D& extent, const std::unique_ptr<swapchain>& old_swapchain = nullptr);

  ~swapchain();

  auto handle() const noexcept -> const VkSwapchainKHR&;

  operator const VkSwapchainKHR&() const noexcept;

  auto extent() const noexcept -> const VkExtent2D&;

  auto active_image_index() const noexcept -> std::uint32_t;

  auto image_count() const noexcept -> std::uint32_t;

  auto pre_transform() const noexcept -> VkSurfaceTransformFlagsKHR;

  auto composite_alpha() const noexcept -> VkCompositeAlphaFlagBitsKHR;

  auto present_mode() const noexcept -> VkPresentModeKHR;

  auto current_framebuffer() const noexcept -> const VkFramebuffer&;

  auto acquire_next_image(const VkSemaphore& image_available_semaphore = nullptr, const VkFence& fence = nullptr) -> VkResult;

  auto queue_present(const VkQueue& queue, const VkSemaphore& wait_semaphore = nullptr) -> VkResult;

private:

  struct depth_image {
    VkImage image{};
    VkDeviceMemory memory{};
    VkImageView image_view{};
  };

  auto _create_image_view(const VkImage& image, VkFormat format, VkImageAspectFlags aspect, VkImageView& image_view) -> void;

  auto _create_depth_images() -> void;

  auto _create_framebuffers() -> void;

  VkExtent2D _extent{};
  VkPresentModeKHR _present_mode{};
  VkFormat _format{};

  std::uint32_t _active_image_index{};
  std::uint32_t _image_count{};

  VkSurfaceTransformFlagsKHR _pre_transform{};
	VkCompositeAlphaFlagBitsKHR _composite_alpha{};

	std::vector<VkImage> _images{};
  std::vector<VkImageView> _image_views{};

  std::vector<depth_image> _depth_images{};

  std::vector<VkFramebuffer> _framebuffers{};

	VkSwapchainKHR _handle{};


}; // class swapchain

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SWAPCHAIN_SWAPCHAIN_HPP_
