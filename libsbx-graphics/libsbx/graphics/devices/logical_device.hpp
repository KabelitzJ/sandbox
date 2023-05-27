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

class logical_device : public utility::noncopyable {

public:

  class queue : public utility::noncopyable {

    friend class logical_device;

  public:

    ~queue() = default;

    auto handle() const noexcept -> const VkQueue&;

    operator const VkQueue&() const noexcept;

    auto family() const noexcept -> std::uint32_t;

  private:

    queue() = default;

    VkQueue _handle{};
    std::uint32_t _family{};

  }; // class queue

  struct sharing_mode {
    VkSharingMode mode{};
    std::vector<std::uint32_t> queue_families{};
  }; // struct sharing_mode

  logical_device(const physical_device& physical_device);

  ~logical_device();

  auto handle() const noexcept -> const VkDevice&;

  operator const VkDevice&() const noexcept;

  auto enables_features() const -> const VkPhysicalDeviceFeatures&;

  auto graphics_queue() const -> const queue&;

  auto compute_queue() const -> const queue&;

  auto transfer_queue() const -> const queue&;

  auto wait_idle() const -> void;

  auto queue_sharing_mode() const noexcept -> const sharing_mode&;

private:

  struct queue_family_indices {
    std::optional<std::uint32_t> graphics{};
    std::optional<std::uint32_t> compute{};
    std::optional<std::uint32_t> transfer{};
  }; // struct queue_family_indices

  auto _get_queue_family_indices(const physical_device& physical_device) const -> queue_family_indices;

  auto _get_enabled_features(const physical_device& physical_device) const -> VkPhysicalDeviceFeatures;

  auto _create_logical_device(const physical_device& physical_device) -> void;

  VkDevice _handle{};
  VkPhysicalDeviceFeatures _enabled_features{};

	queue _graphics_queue{};
	queue _compute_queue{};
	queue _transfer_queue{};
  sharing_mode _queue_sharing_mode{};

}; // class logical_device

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_LOGICAL_DEVICE_HPP_

