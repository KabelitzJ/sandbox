#ifndef LIBSBX_GRAPHICS_INSTANCE_HPP_
#define LIBSBX_GRAPHICS_INSTANCE_HPP_

#include <vulkan/vulkan.hpp>

namespace sbx::graphics {

class instance {

public:

  instance() {

  }

  ~instance() {
    
  }

private:

  VkInstance _handle{};

}; // class instance

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_INSTANCE_HPP_
