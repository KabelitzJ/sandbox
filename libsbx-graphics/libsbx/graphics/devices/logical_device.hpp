#ifndef LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_

#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>

namespace sbx::graphics {

class logical_device {

public:

  struct queue {
    VkQueue handle{};
    std::uint32_t family{};
  }; // struct queue

  logical_device(const physical_device& physical_device);

  ~logical_device();

  auto handle() const noexcept -> const VkDevice&;

  operator const VkDevice&() const noexcept;

  auto enables_features() const -> const VkPhysicalDeviceFeatures&;

  auto graphics_queue() const -> const queue&;
  
  auto present_queue() const -> const queue&;

  auto compute_queue() const -> const queue&;

  auto transfer_queue() const -> const queue&;

  auto wait_idle() const -> void;

private:

  auto _create_queue_indices(const physical_device& physical_device) -> void;

  auto _create_logical_logical_device(const physical_device& physical_device) -> void;

  VkDevice _handle{};
  VkPhysicalDeviceFeatures _enabled_features{};

	VkQueueFlags _supported_queues{};

	queue _graphics_queue{};
	queue _present_queue{};
	queue _compute_queue{};
	queue _transfer_queue{};

}; // class logical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_

