#ifndef DEMO_FENCE_HPP_
#define DEMO_FENCE_HPP_

#include <vulkan/vulkan.hpp>

#include <types/primitives.hpp>

#include "logical_device.hpp"

namespace demo {

class fence {

public:

  fence(logical_device* logical_device, const sbx::uint32 flags = 0)
  : _logical_device{logical_device},
    _handle{nullptr} {
    _initialize(flags);
  }

  fence(const fence&) = delete;

  fence(fence&&) = delete;

  ~fence() {
    _terminate();
  }

  fence& operator=(const fence&) = delete;

  fence& operator=(fence&&) = delete;

  [[nodiscard]] VkFence handle() const noexcept {
    return _handle;
  }

private:

  void _initialize(const sbx::uint32 flags) {
    const auto fence_create_info = VkFenceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags
    };

    if (vkCreateFence(_logical_device->handle(), &fence_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create fence"};
    }
  }

  void _terminate() {
    vkDestroyFence(_logical_device->handle(), _handle, nullptr);
  }

  logical_device* _logical_device{};

  VkFence _handle{};

}; // class fence

} // namespace demo

#endif // DEMO_FENCE_HPP_
