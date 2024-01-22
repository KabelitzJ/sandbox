#ifndef LIBSBX_GRAPHICS_RENDER_PASS_FENCE_HPP_
#define LIBSBX_GRAPHICS_RENDER_PASS_FENCE_HPP_

#include <limits>

#include <vulkan/vulkan.hpp>

namespace sbx::graphics {

class fence final {

public:

  fence(bool is_signaled = true);

  ~fence();

  auto handle() const noexcept -> const VkFence&;

  operator const VkFence&() const noexcept;

  auto wait(std::uint64_t timeout = std::numeric_limits<std::uint64_t>::max()) const noexcept -> void;

  auto reset() const noexcept -> void;

private:

  VkFence _handle;

}; // class fence final

} // sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDER_PASS_FENCE_HPP_