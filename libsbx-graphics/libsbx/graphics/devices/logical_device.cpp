#include <libsbx/graphics/devices/logical_device.hpp>

#include <optional>

#include <libsbx/graphics/devices/extentions.hpp>

namespace sbx::graphics {

logical_device::logical_device(const instance& instance, const physical_device& physical_device) {
  _create_queues(physical_device);
}

logical_device::~logical_device() {

}

VkDevice logical_device::handle() const noexcept {
  return _handle;
}

logical_device::operator VkDevice() const noexcept {
  return _handle;
}

VkPhysicalDeviceFeatures logical_device::physical_device_features() const noexcept {
  return _physical_device_features;
}

const logical_device::queue& logical_device::graphics_queue() const noexcept {
  return _queues.graphics_queue;
}

const logical_device::queue& logical_device::present_queue() const noexcept {
  return _queues.present_queue;
}

const logical_device::queue& logical_device::compute_queue() const noexcept {
  return _queues.compute_queue;
}

const logical_device::queue& logical_device::transfer_queue() const noexcept {
  return _queues.transfer_queue;
}

void logical_device::_create_queues(const physical_device& physical_device) {
  auto device_queue_family_property_count = std::uint32_t{0};
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_property_count, nullptr);
	auto device_queue_family_properties = std::vector<VkQueueFamilyProperties>{device_queue_family_property_count};
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &device_queue_family_property_count, device_queue_family_properties.data());

  auto graphics_family = std::optional<std::uint32_t>{};
  auto present_family = std::optional<std::uint32_t>{};
  auto compute_family = std::optional<std::uint32_t>{};
  auto transfer_family = std::optional<std::uint32_t>{};

  auto index = std::uint32_t{0};

  for (const auto& property : device_queue_family_properties) {
    if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_family = index;
      _queues.graphics_queue.family = index;
      _queues.supported_queues |= VK_QUEUE_GRAPHICS_BIT;
    }

    if (property.queueCount > 0) {
      present_family = index;
      _queues.present_queue.family = index;
    }

    if (property.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      compute_family = index;
      _queues.compute_queue.family = index;
      _queues.supported_queues |= VK_QUEUE_COMPUTE_BIT;
    }

    if (property.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      compute_family = index;
      _queues.compute_queue.family = index;
      _queues.supported_queues |= VK_QUEUE_TRANSFER_BIT;
    }

    if (graphics_family && present_family && compute_family && transfer_family) {
      break;
    }

    ++index;
  }

  if (!graphics_family) {
    throw std::runtime_error{"Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT"};
  }
}

void logical_device::_logical_device() {

}

} // namespace sbx::graphics
