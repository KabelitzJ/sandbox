#ifndef DEMO_SEMAPHORE_HPP_
#define DEMO_SEMAPHORE_HPP_

#include <vulkan/vulkan.hpp>

#include "logical_device.hpp"

namespace demo {

class semaphore {

public:

  semaphore(logical_device* logical_device)
  : _logical_device{logical_device},
    _handle{nullptr} {
    _initialize();
  }

  semaphore(const semaphore&) = delete;

  semaphore(semaphore&&) = delete;

  ~semaphore() {
    _terminate();
  }

  semaphore& operator=(const semaphore&) = delete;

  semaphore& operator=(semaphore&&) = delete;

  [[nodiscard]] VkSemaphore handle() const noexcept {
    return _handle;
  }

  

private:

  void _initialize() {
    const auto semaphore_create_info = VkSemaphoreCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0
    };

    if (vkCreateSemaphore(_logical_device->handle(), &semaphore_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create semaphore"};
    }
  }

  void _terminate() {
    vkDestroySemaphore(_logical_device->handle(), _handle, nullptr);
  }

  logical_device* _logical_device{};

  VkSemaphore _handle{};

}; // class semaphore

} // namespace demo

#endif // DEMO_SEMAPHORE_HPP_
