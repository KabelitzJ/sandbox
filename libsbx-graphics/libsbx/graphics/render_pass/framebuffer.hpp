#ifndef LIBSBX_GRAPHICS_RENDER_PASS_FRAMEBUFFER_HPP_
#define LIBSBX_GRAPHICS_RENDER_PASS_FRAMEBUFFER_HPP_

#include <memory>

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/devices/logical_device.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>
#include <libsbx/graphics/render_pass/render_pass.hpp>

#include <libsbx/graphics/images/image.hpp>

#include <libsbx/graphics/render_stage.hpp>

namespace sbx::graphics {

class framebuffer {

public:

  framebuffer(const VkExtent2D& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

  ~framebuffer();

  auto handle() const noexcept -> const VkFramebuffer&;

  operator const VkFramebuffer&() const noexcept;

  auto color_attachment() const noexcept -> const image&;

  auto depth_attachment() const noexcept -> const image&;

private:

  VkFramebuffer _handle;

  std::unique_ptr<image> _color_attachment;
  std::unique_ptr<image> _depth_attachment;

}; // class framebuffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDER_PASS_FRAMEBUFFER_HPP_
