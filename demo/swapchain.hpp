#ifndef DEMO_SWAPCHAIN_HPP_
#define DEMO_SWAPCHAIN_HPP_

#include <vector>
#include <limits>
#include <algorithm>
#include <ranges>
#include <iostream>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <types/primitives.hpp>
#include <utils/noncopyable.hpp>

#include "enumerate.hpp"

#include "window.hpp"
#include "surface.hpp"
#include "physical_device.hpp"
#include "logical_device.hpp"
#include "command_pool.hpp"

namespace demo {

class swapchain : public sbx::noncopyable {

  inline static constexpr auto max_frames_in_flight = 2;

public:

  swapchain(window* window, surface* surface, physical_device* physical_device, logical_device* logical_device, command_pool* command_pool)
  : _window{window},
    _surface{surface},
    _physical_device{physical_device},
    _logical_device{logical_device},
    _command_pool{command_pool},
    _handle{nullptr},
    _images{},
    _image_views{},
    _format{},
    _extent{} {
    _initialize();
  }

  ~swapchain() {
    _terminate();
  }

  [[nodiscard]] VkSwapchainKHR handle() const noexcept {
    return _handle;
  }

  [[nodiscard]] const std::vector<VkImage>& images() const noexcept {
    return _images;
  }

  [[nodiscard]] const std::vector<VkImageView>& image_views() const noexcept {
    return _image_views;
  }

  [[nodiscard]] VkFormat format() const noexcept {
    return _format;
  }

  [[nodiscard]] const VkExtent2D& extent() const noexcept {
    return _extent;
  }

  [[nodiscard]] VkCommandBuffer current_command_buffer() const noexcept {
    return _frame_data[_current_frame].command_buffer;
  }

  [[nodiscard]] VkFramebuffer current_framebuffer() const noexcept {
    return _framebuffers[_image_index];
  }

  [[nodiscard]] VkRenderPass render_pass() const noexcept {
    return _render_pass;
  }

  void prepare_frame() {
    vkWaitForFences(_logical_device->handle(), 1, &_frame_data[_current_frame].image_in_flight_fence, VK_TRUE, std::numeric_limits<sbx::uint64>::max());

    vkResetFences(_logical_device->handle(), 1, &_frame_data[_current_frame].image_in_flight_fence);

    vkAcquireNextImageKHR(_logical_device->handle(), _handle, std::numeric_limits<sbx::uint64>::max(), _frame_data[_current_frame].image_available_semaphore, nullptr, &_image_index);

    vkResetCommandBuffer(_frame_data[_current_frame].command_buffer, 0);
  }

  void draw_frame() {
    const auto waitStages = std::array<VkPipelineStageFlags, 1>{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    const auto submit_info = VkSubmitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &_frame_data[_current_frame].image_available_semaphore,
      .pWaitDstStageMask = waitStages.data(),
      .commandBufferCount = 1,
      .pCommandBuffers = &_frame_data[_current_frame].command_buffer,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &_frame_data[_current_frame].render_finished_semaphore
    }; 

    if (vkQueueSubmit(_logical_device->graphics_queue(), 1, &submit_info, _frame_data[_current_frame].image_in_flight_fence) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer");
    }

    const auto present_info = VkPresentInfoKHR{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &_frame_data[_current_frame].render_finished_semaphore,
      .swapchainCount = 1,
      .pSwapchains = &_handle,
      .pImageIndices = &_image_index,
      .pResults = nullptr
    };

    vkQueuePresentKHR(_logical_device->present_queue(), &present_info);

    _current_frame = (_current_frame + 1) % max_frames_in_flight;
  }

private:

  struct frame_data {
    VkCommandBuffer command_buffer{};
    VkSemaphore image_available_semaphore{};
    VkSemaphore render_finished_semaphore{};
    VkFence image_in_flight_fence{};
  };

  void _initialize() {
    _create_swapchain();
    _create_render_pass();
    _get_images();
    _create_image_views();
    _create_framebuffers();
    _create_frame_data();
  }

  void _create_swapchain() {
    const auto& swapchain_support = _physical_device->_swapchain_support;

    const auto surface_format = _choose_surface_format(swapchain_support.formats);
    const auto present_mode = _choose_present_mode(swapchain_support.present_modes);
    const auto extent = _choose_extent(swapchain_support.capabilities);

    _image_count = swapchain_support.capabilities.minImageCount + 1;

    if (swapchain_support.capabilities.maxImageCount > 0 && _image_count > swapchain_support.capabilities.maxImageCount) {
      _image_count = swapchain_support.capabilities.maxImageCount;
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
      .minImageCount = _image_count,
      .imageFormat = surface_format.format,
      .imageColorSpace = surface_format.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = indices.size() == 0 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
      .queueFamilyIndexCount = static_cast<sbx::uint32>(indices.size()),
      .pQueueFamilyIndices = indices.size() == 0 ? nullptr : indices.data(),
      .preTransform = swapchain_support.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = present_mode,
      .clipped = true,
      .oldSwapchain = nullptr
    };

    if (vkCreateSwapchainKHR(_logical_device->handle(), &swapchain_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create swapchain!"};
    }

    _format = surface_format.format;
    _extent = extent;
  }

  void _create_render_pass() {
    const auto color_attachment_description = VkAttachmentDescription{
      .flags = 0,
      .format = _format,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    const auto color_attachment_reference = VkAttachmentReference{
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    const auto subpass_description = VkSubpassDescription {
      .flags = 0,
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_attachment_reference,
      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment = nullptr,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr
    };

    const auto subpass_dependency = VkSubpassDependency {
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dependencyFlags = 0
    };

    const auto render_pass_create_info = VkRenderPassCreateInfo {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .attachmentCount = 1,
      .pAttachments = &color_attachment_description,
      .subpassCount = 1,
      .pSubpasses = &subpass_description,
      .dependencyCount = 1,
      .pDependencies = &subpass_dependency
    };

    if (vkCreateRenderPass(_logical_device->handle(), &render_pass_create_info, nullptr, &_render_pass) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create render pass"};
    }
  }

  void _get_images() {
    vkGetSwapchainImagesKHR(_logical_device->handle(), _handle, &_image_count, nullptr);
    _images.resize(_image_count);
    vkGetSwapchainImagesKHR(_logical_device->handle(), _handle, &_image_count, _images.data());
  }

  void _create_image_views() {
    _image_views.resize(_image_count);

    auto index = sbx::uint32{0};

    for (const auto& image : _images) {
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
        throw std::runtime_error{"Failed to create image view!"};
      }

      ++index;
    }
  }

  void _create_framebuffers() {
    _framebuffers.resize(_image_count);

    auto index = sbx::uint32{0};

    for (const auto& image_view : _image_views) {
      const auto framebuffer_info = VkFramebufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = _render_pass,
        .attachmentCount = 1,
        .pAttachments = &image_view,
        .width = _extent.width,
        .height = _extent.height,
        .layers = 1
      };

      if (vkCreateFramebuffer(_logical_device->handle(), &framebuffer_info, nullptr, &_framebuffers[index]) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create framebuffer"};
      }

      ++index;
    }
  }

  void _create_frame_data() {
    _frame_data.resize(max_frames_in_flight);

    const auto semaphore_create_info = VkSemaphoreCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0
    };

    const auto fence_create_info = VkFenceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    const auto command_buffer_allocate_info = VkCommandBufferAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = _command_pool->handle(),
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1
    };

    for (auto& frame : _frame_data) {
      if (vkCreateSemaphore(_logical_device->handle(), &semaphore_create_info, nullptr, &frame.image_available_semaphore) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create semaphore"};
      }

      if (vkCreateSemaphore(_logical_device->handle(), &semaphore_create_info, nullptr, &frame.render_finished_semaphore) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create semaphore"};
      }

      if (vkCreateFence(_logical_device->handle(), &fence_create_info, nullptr, &frame.image_in_flight_fence) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create fence"};
      }

      if (vkAllocateCommandBuffers(_logical_device->handle(), &command_buffer_allocate_info, &frame.command_buffer) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to allocate command buffer"};
      }
    }

  }

  void _terminate() {
    for (const auto& frame : _frame_data) {
      vkDestroyFence(_logical_device->handle(), frame.image_in_flight_fence, nullptr);
      vkDestroySemaphore(_logical_device->handle(), frame.image_available_semaphore, nullptr);
      vkDestroySemaphore(_logical_device->handle(), frame.render_finished_semaphore, nullptr);
      vkFreeCommandBuffers(_logical_device->handle(), _command_pool->handle(), 1, &frame.command_buffer);
    }

    for (const auto& framebuffer : _framebuffers) {
      vkDestroyFramebuffer(_logical_device->handle(), framebuffer, nullptr);
    }

    for (const auto& image_view : _image_views) {
      vkDestroyImageView(_logical_device->handle(), image_view, nullptr);
    }

    vkDestroyRenderPass(_logical_device->handle(), _render_pass, nullptr);

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
  command_pool* _command_pool{};

  sbx::uint32 _image_count{};
  sbx::uint32 _image_index{};
  sbx::uint32 _current_frame{};
  VkSwapchainKHR _handle{};
  VkRenderPass _render_pass{};

  std::vector<VkImage> _images{};
  std::vector<VkImageView> _image_views{};
  std::vector<VkFramebuffer> _framebuffers{};

  std::vector<frame_data> _frame_data{};

  VkFormat _format{};
  VkExtent2D _extent{};

}; // class swapchain

} // namespace demo

#endif // DEMO_SWAPCHAIN_HPP_
