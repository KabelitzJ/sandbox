#ifndef DEMO_RENDER_PASS_HPP_
#define DEMO_RENDER_PASS_HPP_

#include <vulkan/vulkan.hpp>

#include <utils/noncopyable.hpp>

#include "logical_device.hpp"
#include "swapchain.hpp"

namespace demo {

class render_pass : sbx::noncopyable {

public:

  render_pass(logical_device* logical_device, swapchain* swapchain)
  : _logical_device{logical_device},
    _swapchain{swapchain},
    _handle{nullptr} {
    _initialize();
  }

  ~render_pass() {
    _terminate();
  }

  [[nodiscard]] VkRenderPass handle() const noexcept {
    return _handle;
  }

private:

  void _initialize() {
    const auto color_attachment_description = VkAttachmentDescription{
      .flags = 0,
      .format = _swapchain->format(),
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

    if (vkCreateRenderPass(_logical_device->handle(), &render_pass_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create render pass"};
    }
  }

  void _terminate() {
    vkDestroyRenderPass(_logical_device->handle(), _handle, nullptr);
  }

  logical_device* _logical_device{};
  swapchain* _swapchain{};

  VkRenderPass _handle{};

}; // class render_pass

} // namespace demo

#endif // DEMO_RENDER_PASS_HPP_
