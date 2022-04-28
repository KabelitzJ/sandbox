#ifndef DEMO_VALIDATION_LAYERS_HPP_
#define DEMO_VALIDATION_LAYERS_HPP_

#include <string>
#include <vector>

#include <platform/target.hpp>

#include <types/primitives.hpp>

namespace demo {

class validation_layers {

public:

  validation_layers() = delete;

  ~validation_layers() = default;

  [[nodiscard]] static  sbx::uint32 count() noexcept {
#if defined(SBX_DEBUG)
    return static_cast<sbx::uint32>(_layers.size());
#else
    return 0;
#endif
  }

  [[nodiscard]] static const char* const* names() {
#if defined(SBX_DEBUG)
    if (!_support_checked) {
      _check_support();
    }

    return _layers.data();
#else
    return nullptr;
#endif
  }

private:

  static void _check_support() {
    auto available_layer_count = sbx::uint32{0};
    vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);
    auto available_layers = std::vector<VkLayerProperties>{available_layer_count};
    vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data());

    for (const auto* layer_name : _layers) {
      auto found = false;

      for (const auto& properties : available_layers) {
        if (std::strcmp(layer_name, properties.layerName) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        throw std::runtime_error("Validation layers not supported");
      }
    }

    _support_checked = true;
  }

  inline static bool _support_checked{false};

  inline static const std::vector<const char*> _layers{
    "VK_LAYER_KHRONOS_validation"
  };

}; // class validation_layers

} // namespace demo

#endif // DEMO_VALIDATION_LAYERS_HPP_
