#ifndef DEMO_SWAPCHAIN_HPP_
#define DEMO_SWAPCHAIN_HPP_

#include <vulkan/vulkan.hpp>

namespace demo {

class swapchain {

public:

  swapchain()
  : _handle{nullptr} {
    _initialize();
  }

  swapchain(const swapchain&) = delete;

  swapchain(swapchain&&) = delete;

  ~swapchain() {
    _terminate();
  }

  swapchain& operator=(const swapchain&) = delete;

  swapchain& operator=(swapchain&&) = delete;

  [[nodiscard]] VkSwapchainKHR handle() const noexcept {
    return _handle;
  }

private:

  void _initialize() {
    
  }

  void _terminate() {
    
  }

  VkSwapchainKHR _handle{};

}; // class swapchain

} // namespace demo

#endif // DEMO_SWAPCHAIN_HPP_
