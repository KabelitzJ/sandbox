#ifndef DEMO_SURFACE_HPP_
#define DEMO_SURFACE_HPP_

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <utils/noncopyable.hpp>

#include "window.hpp"
#include "instance.hpp"

namespace demo {

class surface : sbx::noncopyable {

public:

  surface(window* window, instance* instance)
  : _window{window},
    _instance{instance},
    _handle{nullptr} {
    _initialize();
  }

  ~surface() {
    _terminate();
  }

  [[nodiscard]] VkSurfaceKHR handle() const noexcept {
    return _handle;
  }

private:

  void _initialize() {
    if (glfwCreateWindowSurface(_instance->handle(), _window->handle(), nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create window surface"};
    }
  }

  void _terminate() {
    vkDestroySurfaceKHR(_instance->handle(), _handle, nullptr);
  }

  window* _window{};
  instance* _instance{};

  VkSurfaceKHR _handle{};

};

} // namespace demo

#endif // DEMO_SURFACE_HPP_
