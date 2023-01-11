#ifndef LIBSBX_GRAPHICS_VALIDATION_LAYERS_HPP_
#define LIBSBX_GRAPHICS_VALIDATION_LAYERS_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

#include <libsbx/core/platform.hpp>

namespace sbx::graphics {

struct validation_layers {

  static auto instance() -> std::vector<const char*> {
#if defined(SBX_DEBUG)
    const auto required_layers = {
      "VK_LAYER_KHRONOS_validation"
    };

    auto available_layer_count = std::uint32_t{0};
    auto available_layers = std::vector<VkLayerProperties>{};

    vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);

    available_layers.resize(available_layer_count);

    vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data());

    for (const auto* required_layer : required_layers) {
      bool found = false;

      for (const auto& available_layer : available_layers) {
        if (std::strcmp(required_layer, available_layer.layerName) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        throw std::runtime_error{"Required layer not available: " + std::string{required_layer}};
      }
    }

    return required_layers;
#else
    return std::vector<const char*>{};
#endif
  }

}; // struct validation_layers

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_VALIDATION_LAYERS_HPP_
