#ifndef LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>

namespace sbx::graphics {

class logical_device : public utility::noncopyable {

public:

  struct queue {
    std::uint32_t family{};
    VkQueue handle{};
  }; // struct queue

  logical_device(const physical_device& physical_device);

  ~logical_device();

  VkDevice handle() const noexcept;

  operator VkDevice() const noexcept;

  VkPhysicalDeviceFeatures physical_device_features() const noexcept;

  const queue& graphics_queue() const noexcept;

  const queue& present_queue() const noexcept;

  const queue& compute_queue() const noexcept;

  const queue& transfer_queue() const noexcept;

  VkResult wait_idle();

private:

  struct queues {
    VkQueueFlags supported_queues{};
    queue graphics_queue{};  
    queue present_queue{};  
    queue compute_queue{};  
    queue transfer_queue{};  
  };

  void _create_queues(const physical_device& physical_device);
  void _create_logical_device(const physical_device& physical_device);

  VkDevice _handle{};
  VkPhysicalDeviceFeatures _physical_device_features{};

  queues _queues{};

}; // class logical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_
