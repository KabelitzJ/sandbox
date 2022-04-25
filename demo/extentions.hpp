#ifndef DEMO_EXTENTIONS_HPP_
#define DEMO_EXTENTIONS_HPP_

#include <vector>

#include <GLFW/glfw3.h>

#include <platform/target.hpp>

#include <types/primitives.hpp>

namespace demo {

class extentions {

public:

  extentions()
  : _extentions{_initialize_extentions()} { }

  ~extentions() = default;

  [[nodiscard]] sbx::uint32 count() const noexcept {
    return static_cast<sbx::uint32>(_extentions.size());
  }

  [[nodiscard]] const char* const* names() const {
    return _extentions.data();
  }

private:

  std::vector<const char*> _initialize_extentions() {
    auto glfw_extention_count = sbx::uint32{0};
    const auto** glfw_extentions = glfwGetRequiredInstanceExtensions(&glfw_extention_count);

    auto required_extentions = std::vector<const char*>{glfw_extentions, glfw_extentions + glfw_extention_count};

#if defined(SBX_DEBUG)
    required_extentions.reserve(required_extentions.size() + _validation_extentions.size());
    required_extentions.insert(required_extentions.end(), _validation_extentions.begin(), _validation_extentions.end());
#endif

    return required_extentions;
  }

  inline static const std::vector<const char*> _validation_extentions{
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
  };

  std::vector<const char*> _extentions{};

};

} // namespace demo

#endif // DEMO_EXTENTIONS_HPP_
