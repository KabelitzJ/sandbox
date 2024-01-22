#ifndef LIBSBX_GRAPHICS_RENDER_PASS_SEMAPHORE_HPP_
#define LIBSBX_GRAPHICS_RENDER_PASS_SEMAPHORE_HPP_

#include <vulkan/vulkan.hpp>

namespace sbx::graphics {

class semaphore final {

public:

  semaphore();

  ~semaphore();

  auto handle() const noexcept -> const VkSemaphore&;

  operator const VkSemaphore&() const noexcept;

private:

  VkSemaphore _handle;

}; // class fence final

} // sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDER_PASS_SEMAPHORE_HPP_