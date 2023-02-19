#ifndef LIBSBX_GRAPHICS_RENDERPASS_RENDERPASS_HPP_
#define LIBSBX_GRAPHICS_RENDERPASS_RENDERPASS_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::graphics {

class renderpass : public utility::noncopyable {
  
public:

  renderpass();

  ~renderpass();

  auto handle() const noexcept -> const VkRenderPass&;

  operator const VkRenderPass&() const noexcept;

private:

  VkRenderPass _handle{};

}; // class renderpass

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDERPASS_RENDERPASS_HPP_
