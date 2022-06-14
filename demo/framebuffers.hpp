#ifndef DEMO_FRAMEBUFFERS_HPP_
#define DEMO_FRAMEBUFFERS_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

#include "logical_device.hpp"
#include "swapchain.hpp"
#include "render_pass.hpp"

#include "enumerate.hpp"

namespace demo {

class framebuffers {

public:

  framebuffers(logical_device* logical_device, swapchain* swapchain, render_pass* render_pass)
  : _logical_device{logical_device},
    _swapchain{swapchain},
    _render_pass{render_pass},
    _framebuffers{} {
    _initialize();
  }

  framebuffers(const framebuffers&) = delete;

  framebuffers(framebuffers&&) = delete;

  ~framebuffers() {
    _terminate();
  }

  framebuffers& operator=(const framebuffers&) = delete;

  framebuffers& operator=(framebuffers&&) = delete;

  [[nodiscard]] const VkFramebuffer* data() const noexcept {
    return _framebuffers.data();	
  }

  [[nodiscard]] std::size_t size() const noexcept {
    return _framebuffers.size();
  }

private:

  void _initialize() {
    const auto& image_views = _swapchain->image_views();
    const auto& extend = _swapchain->extent();

    _framebuffers.resize(image_views.size());

    for (const auto& [image_view, index] : enumerate(image_views)) {
      const auto framebuffer_info = VkFramebufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = _render_pass->handle(),
        .attachmentCount = 1,
        .pAttachments = &image_view,
        .width = extend.width,
        .height = extend.height,
        .layers = 1
      };

      if (vkCreateFramebuffer(_logical_device->handle(), &framebuffer_info, nullptr, &_framebuffers[index]) != VK_SUCCESS) {
        throw std::runtime_error{"failed to create framebuffer"};
      }
    }
  }

  void _terminate() {
    for (const auto& framebuffer : _framebuffers) {
      vkDestroyFramebuffer(_logical_device->handle(), framebuffer, nullptr);
    }
  }

  logical_device* _logical_device{};
  swapchain* _swapchain{};
  render_pass* _render_pass{};

  std::vector<VkFramebuffer> _framebuffers{};

}; // class framebuffers

} // namespace demo

#endif // DEMO_FRAMEBUFFERS_HPP_
