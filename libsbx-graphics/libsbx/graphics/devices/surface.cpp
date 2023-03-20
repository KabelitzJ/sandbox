#include <libsbx/graphics/devices/surface.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

surface::surface(const instance& instance, const physical_device& physical_device, const logical_device& logical_device) {
  auto& window = devices::devices_module::get().window();

  validate(glfwCreateWindowSurface(instance, window, nullptr, &_handle));

  validate(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, _handle, &_capabilities));

  auto surface_format_count = std::uint32_t{0};
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, _handle, &surface_format_count, nullptr);
  
	auto surface_formats = std::vector<VkSurfaceFormatKHR>{surface_format_count};
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, _handle, &surface_format_count, surface_formats.data());

  if (surface_formats.empty()) {
    throw std::runtime_error("No surface formats available");
  }

  auto found_desired_format = false;

  for (auto& surface_format : surface_formats) {
    if (surface_format.format == VK_FORMAT_B8G8R8A8_SRGB && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      _format = surface_format;
      found_desired_format = true;

      break;
    }
  }

  if (!found_desired_format) {
    _format = surface_formats[0];
  }

	auto present_support = std::uint32_t{0};
	vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, logical_device.graphics_queue().family(), _handle, &present_support);

	if (!present_support) {
		throw std::runtime_error("Graphics queue family does not have presentation support");
  }
}

surface::~surface() {
  auto& instance = graphics_module::get().instance();
  vkDestroySurfaceKHR(instance, _handle, nullptr);
}

auto surface::handle() const noexcept -> const VkSurfaceKHR& {
  return _handle;
}

surface::operator const VkSurfaceKHR&() const noexcept {
  return _handle;
}

auto surface::capabilities() const noexcept -> const VkSurfaceCapabilitiesKHR& {
  return _capabilities;
}

auto surface::format() const noexcept -> const VkSurfaceFormatKHR& {
  return _format;
}

} // namespace sbx::graphics
