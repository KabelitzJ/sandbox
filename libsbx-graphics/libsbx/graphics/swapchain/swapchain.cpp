#include <libsbx/graphics/swapchain/swapchain.hpp>

#include <limits>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/images/image.hpp>

namespace sbx::graphics {

static const auto composite_alpha_flags = std::vector<VkCompositeAlphaFlagBitsKHR>{
	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, 
  VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
	VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, 
  VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
};

swapchain::swapchain(const extent2d& extent, const std::unique_ptr<swapchain>& old_swapchain)
: _extent{extent},
  _present_mode{VK_PRESENT_MODE_FIFO_KHR},
  _active_image_index{std::numeric_limits<std::uint32_t>::max()},
  _pre_transform{VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR},
  _composite_alpha{VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR} {
  const auto& surface = graphics_module::get().surface();
  const auto& physical_device = graphics_module::get().physical_device();
  const auto& logical_device = graphics_module::get().logical_device();

  const auto& surface_capabilities = surface.capabilities();
  const auto& surface_format = surface.format();

  const auto& graphics_queue = logical_device.graphics_queue();
  const auto& present_queue = logical_device.present_queue();

  auto physical_present_mode_count = std::uint32_t{0};
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &physical_present_mode_count, nullptr);

  auto physical_present_modes = std::vector<VkPresentModeKHR>{physical_present_mode_count};
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &physical_present_mode_count, physical_present_modes.data());

  for (const auto& present_mode : physical_present_modes) {
		if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			_present_mode = present_mode;
			break;
		}

		if (present_mode != VK_PRESENT_MODE_MAILBOX_KHR && present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			_present_mode = present_mode;
		}
	}

  auto desired_image_count = surface_capabilities.minImageCount + 1;

	if (surface_capabilities.maxImageCount > 0 && desired_image_count > surface_capabilities.maxImageCount) {
		desired_image_count = surface_capabilities.maxImageCount;
	}

	if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		_pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	} else {
		_pre_transform = surface_capabilities.currentTransform;
	}

	for (const auto& composite_alpha_flag : composite_alpha_flags) {
		if (surface_capabilities.supportedCompositeAlpha & composite_alpha_flag) {
			_composite_alpha = composite_alpha_flag;
			break;
		}
	}

	auto swapchain_create_info = VkSwapchainCreateInfoKHR{};
	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.surface = surface;
	swapchain_create_info.minImageCount = desired_image_count;
	swapchain_create_info.imageFormat = surface_format.format;
	swapchain_create_info.imageColorSpace = surface_format.colorSpace;
	swapchain_create_info.imageExtent = _extent;
	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_create_info.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(_pre_transform);
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.compositeAlpha = _composite_alpha;
	swapchain_create_info.presentMode = _present_mode;
	swapchain_create_info.clipped = true;
	swapchain_create_info.oldSwapchain = nullptr;

	if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
		swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

	if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
		swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

	if (old_swapchain) {
    swapchain_create_info.oldSwapchain = *old_swapchain;
  }

	if (graphics_queue.family() != present_queue.family()) {
		auto queue_family = std::array<uint32_t, 2>{graphics_queue.family(), present_queue.family()};
		swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_family.size());
		swapchain_create_info.pQueueFamilyIndices = queue_family.data();
	}

	validate(vkCreateSwapchainKHR(logical_device, &swapchain_create_info, nullptr, &_handle));

	validate(vkGetSwapchainImagesKHR(logical_device, _handle, &_image_count, nullptr));

	_images.resize(_image_count);
	_image_views.resize(_image_count);

	validate(vkGetSwapchainImagesKHR(logical_device, _handle, &_image_count, _images.data()));

	for (uint32_t i = 0; i < _image_count; i++) {
    image::create_image_view(_images.at(i), _image_views.at(i), VK_IMAGE_VIEW_TYPE_2D, surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);
	}

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	vkCreateFence(logical_device, &fenceCreateInfo, nullptr, &_image_fence);
}

swapchain::~swapchain() {
	const auto& logical_device = graphics_module::get().logical_device();

	vkDestroySwapchainKHR(logical_device, _handle, nullptr);

	for (const auto& image_view : _image_views) {
		vkDestroyImageView(logical_device, image_view, nullptr);
	}

	vkDestroyFence(logical_device, _image_fence, nullptr);
}

auto swapchain::handle() const noexcept -> const VkSwapchainKHR& {
  return _handle;
}

swapchain::operator const VkSwapchainKHR&() const noexcept {
  return _handle;
}

auto swapchain::extent() const noexcept -> const extent2d& {
  return _extent;
}

auto swapchain::active_image_index() const noexcept -> std::uint32_t {
  return _active_image_index;
}

auto swapchain::image_count() const noexcept -> std::uint32_t {
  return _image_count;
}

auto swapchain::pre_transform() const noexcept -> VkSurfaceTransformFlagsKHR {
  return _pre_transform;
}

auto swapchain::composite_alpha() const noexcept -> VkCompositeAlphaFlagBitsKHR {
  return _composite_alpha;
}

auto swapchain::present_mode() const noexcept -> VkPresentModeKHR {
  return _present_mode;
}

auto swapchain::images() const noexcept -> const std::vector<VkImage>& {
  return _images;
}

auto swapchain::image(std::uint32_t index) const noexcept -> const VkImage& {
  return _images.at(index);
}

auto swapchain::image_views() const noexcept -> const std::vector<VkImageView>& {
  return _image_views;
}

auto swapchain::image_view(std::uint32_t index) const noexcept -> const VkImageView& {
  return _image_views.at(index);
}

auto swapchain::acquire_next_image(const VkSemaphore& image_available_semaphore, const VkFence& fence) -> VkResult {
  auto& logical_device = graphics_module::get().logical_device();

  if (fence) {
    validate(vkWaitForFences(logical_device, 1, &fence, true, std::numeric_limits<std::uint64_t>::max()));
  }

  const auto result = vkAcquireNextImageKHR(logical_device, _handle, std::numeric_limits<std::uint64_t>::max(), image_available_semaphore, nullptr, &_active_image_index);

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR) {
    throw std::runtime_error("Failed to acquire next image");
  }

  return result;
}

auto swapchain::queue_present(const VkQueue& queue, const VkSemaphore& wait_semaphore) -> VkResult {
  auto present_info = VkPresentInfoKHR{};

	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &wait_semaphore;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &_handle;
	present_info.pImageIndices = &_active_image_index;

	return vkQueuePresentKHR(queue, &present_info);
}

} // namespace sbx::graphics
