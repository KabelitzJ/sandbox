#ifndef DEMO_SWAPCHAIN_HPP_
#define DEMO_SWAPCHAIN_HPP_

#include <vector>
#include <limits>
#include <algorithm>
#include <ranges>

#include <types/primitives.hpp>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "enumerate.hpp"

#include "window.hpp"
#include "surface.hpp"
#include "physical_device.hpp"
#include "logical_device.hpp"

namespace demo {

class swapchain {

public:

  swapchain(window* window, surface* surface, physical_device* physical_device, logical_device* logical_device)
  : _window{window},
    _surface{surface},
    _physical_device{physical_device},
    _logical_device{logical_device},
    _handle{nullptr},
    _images{},
    _image_views{},
    _framebuffers{},
    _format{},
    _extent{} {
    _initialize();
  }

  swapchain(const swapchain&) = delete;

  swapchain(swapchain&&) = delete;

  ~swapchain() {
    _terminate();
  }

  swapchain& operator=(const swapchain&) = delete;

  swapchain& operator=(swapchain&&) = delete;

  [[nodiscard]] VkSwapchainKHR handle() const noexcept {
    return _handle;
  }

  [[nodiscard]] const std::vector<VkImage>& images() const noexcept {
    return _images;
  }

  [[nodiscard]] const VkFormat& format() const noexcept {
    return _format;
  }

  [[nodiscard]] const VkExtent2D& extent() const noexcept {
    return _extent;
  }

private:

  void _initialize() {
    const auto& swapchain_support = _physical_device->_swapchain_support;

    const auto surface_format = _choose_surface_format(swapchain_support.formats);
    const auto present_mode = _choose_present_mode(swapchain_support.present_modes);
    const auto extent = _choose_extent(swapchain_support.capabilities);

    auto image_count = swapchain_support.capabilities.minImageCount + 1;

    if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount) {
      image_count = swapchain_support.capabilities.maxImageCount;
    }

    const auto& queue_family_indices = _physical_device->_queue_family_indices;

    auto indices = std::vector<sbx::uint32>{};

    if (queue_family_indices.graphics_family.value() != queue_family_indices.present_family.value()) {
      indices.push_back(queue_family_indices.graphics_family.value());
      indices.push_back(queue_family_indices.present_family.value());
    }

    const auto swapchain_create_info = VkSwapchainCreateInfoKHR{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .surface = _surface->handle(),
      .minImageCount = image_count,
      .imageFormat = surface_format.format,
      .imageColorSpace = surface_format.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = indices.size() == 0 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
      .queueFamilyIndexCount = static_cast<sbx::uint32>(indices.size()),
      .pQueueFamilyIndices = indices.data(),
      .preTransform = swapchain_support.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = present_mode,
      .clipped = true,
      .oldSwapchain = nullptr
    };

    if (vkCreateSwapchainKHR(_logical_device->handle(), &swapchain_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"failed to create swapchain!"};
    }

    vkGetSwapchainImagesKHR(_logical_device->handle(), _handle, &image_count, nullptr);
    _images.resize(image_count);
    vkGetSwapchainImagesKHR(_logical_device->handle(), _handle, &image_count, _images.data());

    _format = surface_format.format;
    _extent = extent;

    _image_views.resize(image_count);

    for (const auto& [image, index] : enumerate(_images)) {
      const auto image_view_create_info = VkImageViewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = _format,
        .components = {
          .r = VK_COMPONENT_SWIZZLE_IDENTITY,
          .g = VK_COMPONENT_SWIZZLE_IDENTITY,
          .b = VK_COMPONENT_SWIZZLE_IDENTITY,
          .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1
        }
      };

      if (vkCreateImageView(_logical_device->handle(), &image_view_create_info, nullptr, &_image_views[index]) != VK_SUCCESS) {
        throw std::runtime_error{"failed to create image view!"};
      }
    }
  }

  void _terminate() {
    for (const auto& framebuffer : _framebuffers) {
      vkDestroyFramebuffer(_logical_device->handle(), framebuffer, nullptr);
    }

    for (const auto& image_view : _image_views) {
      vkDestroyImageView(_logical_device->handle(), image_view, nullptr);
    }

    vkDestroySwapchainKHR(_logical_device->handle(), _handle, nullptr);
  }

  VkSurfaceFormatKHR _choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) {
    if (available_formats.size() == 1) {
      return available_formats[0];
    }

    for (const auto& format : available_formats) {
      if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return format;
      }
    }

    return available_formats[0];
  }

  VkPresentModeKHR _choose_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) {
    if (available_present_modes.size() == 1) {
      return available_present_modes[0];
    }

    for (const auto& present_mode : available_present_modes) {
      if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return present_mode;
      }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D _choose_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<sbx::uint32>::max()) {
      return capabilities.currentExtent;
    }

    auto width = sbx::int32{0};
    auto height = sbx::int32{0};

    glfwGetFramebufferSize(_window->handle(), &width, &height);

    VkExtent2D actual_extent = {
      static_cast<sbx::uint32>(width),
      static_cast<sbx::uint32>(height)
    };

    actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
  }

  window* _window{};
  surface* _surface{};
  physical_device* _physical_device{};
  logical_device* _logical_device{};

  VkSwapchainKHR _handle{};
  std::vector<VkImage> _images{};
  std::vector<VkImageView> _image_views{};
  std::vector<VkFramebuffer> _framebuffers{};
  VkFormat _format{};
  VkExtent2D _extent{};

}; // class swapchain

} // namespace demo

#endif // DEMO_SWAPCHAIN_HPP_
