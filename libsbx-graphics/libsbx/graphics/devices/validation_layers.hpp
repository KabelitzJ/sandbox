#ifndef LIBSBX_GRAPHICS_DEVICES_VALIDATION_LAYERS_HPP_
#define LIBSBX_GRAPHICS_DEVICES_VALIDATION_LAYERS_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/target.hpp>

namespace sbx::graphics {

struct validation_layers {

  static auto instance() -> std::vector<const char*> {
    auto required_layers = std::vector<const char*>{};

    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      required_layers.push_back("VK_LAYER_KHRONOS_validation");

      auto available_layer_count = std::uint32_t{0};
      vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);

      auto available_layers = std::vector<VkLayerProperties>{available_layer_count};
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
    }

    return required_layers;
  }

}; // struct validation_layers

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_VALIDATION_LAYERS_HPP_
