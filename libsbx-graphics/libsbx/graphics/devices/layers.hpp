#ifndef LIBSBX_GRAPHICS_DEVICES_LAYERS_HPP_
#define LIBSBX_GRAPHICS_DEVICES_LAYERS_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

#include <libsbx/core/platform.hpp>

namespace sbx::graphics {

struct layers {
  static std::vector<const char*> instance() noexcept {
    auto layers = std::vector<const char*>{};
#if defined(LIBSBX_DEBUG)
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
    return layers;
  }
}; // struct layers

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_LAYERS_HPP_
