#ifndef DEMO_SEMAPHORE_HPP_
#define DEMO_SEMAPHORE_HPP_

#include <vulkan/vulkan.hpp>

#include <utils/noncopyable.hpp>

#include "logical_device.hpp"

namespace demo {

class semaphore : sbx::noncopyable {

public:

  semaphore(logical_device* logical_device, const sbx::uint32 flags = 0)
  : _logical_device{logical_device},
    _handle{nullptr} {
    _initialize(flags);
  }

  ~semaphore() {
    _terminate();
  }

  [[nodiscard]] VkSemaphore handle() const noexcept {
    return _handle;
  }

private:

  void _initialize(const sbx::uint32 flags) {
    const auto semaphore_create_info = VkSemaphoreCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags
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
