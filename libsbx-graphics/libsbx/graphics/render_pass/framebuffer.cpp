#include <libsbx/graphics/render_pass/framebuffer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

framebuffer::framebuffer(const VkExtent2D& extent, VkSampleCountFlagBits samples) {
  auto& logical_device = graphics_module::get().logical_device();
  auto& physical_device = graphics_module::get().physical_device();
  auto& surface = graphics_module::get().surface();
  // auto& render_pass = graphics_module::get().render_pass();

  _color_attachment = std::make_unique<image>(
    VK_IMAGE_TYPE_2D, 
    VkExtent3D{
      extent.width, 
      extent.height, 
      1
    }, 
    VK_SAMPLE_COUNT_1_BIT, 
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
    surface.format().format, 
    1,
    1, 
    VK_IMAGE_LAYOUT_UNDEFINED
  );

  auto depth_format = physical_device.find_supported_format(
    {
      VK_FORMAT_D32_SFLOAT, 
      VK_FORMAT_D32_SFLOAT_S8_UINT, 
      VK_FORMAT_D24_UNORM_S8_UINT
    },
    VK_IMAGE_TILING_OPTIMAL,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
  );

  _depth_attachment = std::make_unique<image>(
    VK_IMAGE_TYPE_2D, 
    VkExtent3D{
      extent.width, 
      extent.height, 
      1
    }, 
    VK_SAMPLE_COUNT_1_BIT, 
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
    depth_format, 
    1,
    1, 
    VK_IMAGE_LAYOUT_UNDEFINED
  );

  auto attachments = std::array<VkImageView, 2>{
    _color_attachment->view(), 
    _depth_attachment->view()
  };

  auto framebuffer_create_info = VkFramebufferCreateInfo{};
  framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  // framebuffer_create_info.renderPass = render_pass;
  framebuffer_create_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
  framebuffer_create_info.pAttachments = attachments.data();
  framebuffer_create_info.width = extent.width;
  framebuffer_create_info.height = extent.height;
  framebuffer_create_info.layers = 1;

  validate(vkCreateFramebuffer(logical_device.handle(), &framebuffer_create_info, nullptr, &_handle));
}

framebuffer::~framebuffer() {
  auto& logical_device = graphics_module::get().logical_device();

  vkDestroyFramebuffer(logical_device.handle(), _handle, nullptr);
}

auto framebuffer::handle() const noexcept -> const VkFramebuffer& {
  return _handle;
}

framebuffer::operator const VkFramebuffer&() const noexcept {
  return _handle;
}

auto framebuffer::color_attachment() const noexcept -> const image& {
  return *_color_attachment;
}

auto framebuffer::depth_attachment() const noexcept -> const image& {
  return *_depth_attachment;
}

} // namespace sbx::graphics
