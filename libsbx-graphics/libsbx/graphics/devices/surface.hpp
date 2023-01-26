#ifndef LIBSBX_GRAPHICS_DEVICES_SURFACE_HPP_
#define LIBSBX_GRAPHICS_DEVICES_SURFACE_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>

namespace sbx::graphics {

class surface {

public:

  surface(const instance& instance, const physical_device& physical_device, const logical_device& logical_device);

  ~surface();

  auto handle() const noexcept -> VkSurfaceKHR;

  operator VkSurfaceKHR() const noexcept;

private:

  VkSurfaceKHR _handle{};
	VkSurfaceCapabilitiesKHR _capabilities{};
	VkSurfaceFormatKHR _format{};

}; // class surface

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_SURFACE_HPP_
