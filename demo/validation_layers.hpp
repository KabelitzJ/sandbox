#ifndef DEMO_VALIDATION_LAYERS_HPP_
#define DEMO_VALIDATION_LAYERS_HPP_

#include <array>

#include <types/primitives.hpp>

namespace demo {

class validation_layers {

public:

  validation_layers() = default;

  ~validation_layers() = default;

  [[nodiscard]] static constexpr sbx::uint32 count() noexcept {
#ifdef NDEBUG
    return 0;
#else
    return static_cast<sbx::uint32>(_layer_names.size());
#endif
  }

  [[nodiscard]] static constexpr const char* const* names() noexcept {
#ifdef NDEBUG
    return nullptr;
#else
    return _layer_names.data();
#endif
  }

private:

  inline static constexpr auto _layer_names = std::array<const char*, 1>{
    "VK_LAYER_KHRONOS_validation"
  };

}; // class validation_layers

} // namespace demo

#endif // DEMO_VALIDATION_LAYERS_HPP_
