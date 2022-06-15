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

  /**
   * @brief Return a pointer to the first framebuffer element.
   * 
   * @return const VkFramebuffer* A pointer to the first framebuffer element.
   */
  [[nodiscard]] const VkFramebuffer* data() const noexcept {
    return _framebuffers.data();	
  }

  /**
   * @brief Return the number of framebuffers.
   * 
   * @return size_t The number of framebuffers.
   */
  [[nodiscard]] std::size_t size() const noexcept {
    return _framebuffers.size();
  }

  /**
   * @brief Get the framebuffer for the given index. Does not check if the index is valid.
   * 
   * @param index The index of the framebuffer.
   * 
   * @return VkFramebuffer The framebuffer at the given index.
   */
  [[nodiscard]] VkFramebuffer operator[](std::size_t index) noexcept {
    return _framebuffers[index];
  }

  /**
   * @brief Get the framebuffer at the given index.
   * 
   * @param index The index of the framebuffer to retrieve.
   * 
   * @return VkFramebuffer The framebuffer at the given index.
   * 
   * @throws std::out_of_range if the index is out of range.
   */
  [[nodiscard]] VkFramebuffer at(std::size_t index) {
    if (index >= _framebuffers.size()) {
      throw std::out_of_range{"Framebuffer index out of range"};
    }

    return _framebuffers.at(index);
  }

private:

  void _initialize() {
    const auto& image_views = _swapchain->image_views();
    const auto& extent = _swapchain->extent();

    _framebuffers.resize(image_views.size());

    for (const auto& [image_view, index] : enumerate(image_views)) {
      const auto framebuffer_info = VkFramebufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = _render_pass->handle(),
        .attachmentCount = 1,
        .pAttachments = &image_view,
        .width = extent.width,
        .height = extent.height,
        .layers = 1
      };

      if (vkCreateFramebuffer(_logical_device->handle(), &framebuffer_info, nullptr, &_framebuffers[index]) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create framebuffer"};
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
