#ifndef DEMO_COMMAND_POOL_HPP_
#define DEMO_COMMAND_POOL_HPP_

#include <vulkan/vulkan.hpp>

#include "physical_device.hpp"
#include "logical_device.hpp"

namespace demo {

class command_pool {

public:

  command_pool(physical_device* physical_device, logical_device* logical_device)
  : _physical_device{physical_device},
    _logical_device{logical_device},
    _handle{nullptr} {
    _initialize();
  }

  command_pool(const command_pool&) = delete;

  command_pool(command_pool&&) = delete;

  ~command_pool() {
    _terminate();
  }

  command_pool& operator=(const command_pool&) = delete;

  command_pool& operator=(command_pool&&) = delete;

  [[nodiscard]] VkCommandPool handle() const noexcept {
    return _handle;
  }

private:

  void _initialize() {
    const auto graphics_queue_family_index = _physical_device->_queue_family_indices.graphics_family.value();

    const auto command_pool_create_info = VkCommandPoolCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = graphics_queue_family_index
    };

    if (vkCreateCommandPool(_logical_device->handle(), &command_pool_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create command pool"};
    }
  }

  void _terminate() {
    vkDestroyCommandPool(_logical_device->handle(), _handle, nullptr);
  }

  physical_device* _physical_device{};
  logical_device* _logical_device{};

  VkCommandPool _handle{};

}; // class command_pool

} // namespace demo

#endif // DEMO_COMMAND_POOL_HPP_
