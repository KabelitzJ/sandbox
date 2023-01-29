#ifndef LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/devices/debug_messenger.hpp>

namespace sbx::graphics {

class instance {

  using debug_messenger_type = debug_messenger<instance>;

public:

  instance();

  ~instance();

  auto handle() const noexcept -> const VkInstance&;

  operator const VkInstance&() const noexcept;

private:

  VkInstance _handle{};

}; // class instance

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_INSTANCE_HPP_
