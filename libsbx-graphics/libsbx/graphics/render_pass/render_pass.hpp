#ifndef LIBSBX_GRAPHICS_RENDER_PASS_RENDER_PASS_HPP_
#define LIBSBX_GRAPHICS_RENDER_PASS_RENDER_PASS_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::graphics {

class render_pass : public utility::noncopyable {
  
public:

  render_pass();

  ~render_pass();

  auto handle() const noexcept -> const VkRenderPass&;

  operator const VkRenderPass&() const noexcept;

private:

  VkRenderPass _handle{};

}; // class render_pass

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDER_PASS_RENDER_PASS_HPP_
