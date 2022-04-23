#ifndef DEMO_VALIDATION_LAYERS_HPP_
#define DEMO_VALIDATION_LAYERS_HPP_

#include <string>
#include <vector>

#include <types/primitives.hpp>

namespace demo {

class validation_layers {

public:

  validation_layers() = default;

  ~validation_layers() = default;

  [[nodiscard]] sbx::uint32 count() const noexcept {
#ifdef NDEBUG
    return 0;
#else
    return static_cast<sbx::uint32>(_layers.size());
#endif
  }

  [[nodiscard]] const char* const* names() const {
#ifdef NDEBUG
    return nullptr;
#else
    if (!_check_support()) {
      throw std::runtime_error("Validation layers not supported");
    }

    return _layers.data();
#endif
  }

private:

  bool _check_support() const {
    auto available_layer_count = sbx::uint32{0};
    vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);
    auto available_layers = std::vector<VkLayerProperties>{available_layer_count};
    vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data());

    for (const auto* layer_name : _layers) {
      auto found = false;

      for (const auto& properties : available_layers) {
        if (strcmp(layer_name, properties.layerName) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        return false;
      }
    }

    return true;
  }

  inline static const std::vector<const char*> _layers{
    "VK_LAYER_KHRONOS_validation"
  };

}; // class validation_layers

} // namespace demo

#endif // DEMO_VALIDATION_LAYERS_HPP_
