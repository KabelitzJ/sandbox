#ifndef LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_

#include <unordered_map>
#include <optional>
#include <cinttypes>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>

namespace sbx::graphics {

class queue : public utility::noncopyable {

  friend class logical_device;

public:

  enum class type : std::uint32_t {
    graphics,
    present,
    compute,
    transfer
  }; // enum class type

  queue(queue&& other) noexcept = default;

  ~queue() = default;

  auto operator=(queue&& other) noexcept -> queue& = default;

  auto handle() const noexcept -> const VkQueue&;

  operator const VkQueue&() const noexcept;

  auto family() const noexcept -> std::uint32_t;

  auto wait_idle() const -> void;

private:

  queue(const VkQueue& handle, std::uint32_t family)
  : _handle{handle},
    _family{family} { }

  VkQueue _handle{};
  std::uint32_t _family{};

}; // class queue

class logical_device : public utility::noncopyable {

public:

  logical_device(const physical_device& physical_device);

  ~logical_device();

  auto handle() const noexcept -> const VkDevice&;

  operator const VkDevice&() const noexcept;

  auto enabled_features() const -> const VkPhysicalDeviceFeatures&;

  template<queue::type Type>
  auto queue() const -> const graphics::queue& {
    return _queues.at(Type);
  }

  auto wait_idle() const -> void;

private:

  struct queue_family_indices {
    std::optional<std::uint32_t> graphics{};
    std::optional<std::uint32_t> present{};
    std::optional<std::uint32_t> compute{};
    std::optional<std::uint32_t> transfer{};
  }; // struct queue_family_indices

  auto _get_queue_family_indices(const physical_device& physical_device) const -> queue_family_indices;

  auto _get_enabled_features(const physical_device& physical_device) const -> VkPhysicalDeviceFeatures;

  auto _create_logical_device(const physical_device& physical_device) -> void;

  VkDevice _handle{};
  VkPhysicalDeviceFeatures _enabled_features{};
  std::unordered_map<queue::type, graphics::queue> _queues{};

}; // class logical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_

