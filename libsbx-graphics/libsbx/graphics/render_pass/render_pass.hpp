#ifndef LIBSBX_GRAPHICS_RENDER_PASS_RENDER_PASS_HPP_
#define LIBSBX_GRAPHICS_RENDER_PASS_RENDER_PASS_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/graphics/devices/physical_device.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>
#include <libsbx/graphics/devices/surface.hpp>

namespace sbx::graphics {

class render_pass : public utility::noncopyable {
  
public:

  render_pass(const physical_device& physical_device, const logical_device& logical_device, const surface& surface);

  ~render_pass();

  auto handle() const noexcept -> const VkRenderPass&;

  operator const VkRenderPass&() const noexcept;

private:

  VkRenderPass _handle{};

}; // class render_pass

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDER_PASS_RENDER_PASS_HPP_
