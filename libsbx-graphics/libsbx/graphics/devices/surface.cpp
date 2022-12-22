#include <libsbx/graphics/devices/surface.hpp>

#include <vulkan/vulkan.hpp>

#include <libsbx/devices/device_module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

surface::surface(const instance& instance, const physical_device& physical_device, const logical_device& logical_device) {
  auto& window = devices::device_module::get().window();

  graphics_module::validate(glfwCreateWindowSurface(instance, window, nullptr, &_handle));

  graphics_module::validate(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, _handle, &_capabilities));

  auto surface_format_count = std::uint32_t{0};
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, _handle, &surface_format_count, nullptr);
	auto surface_formats = std::vector<VkSurfaceFormatKHR>{surface_format_count};
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, _handle, &surface_format_count, surface_formats.data());

	if (surface_format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED) {
		_format.format = VK_FORMAT_B8G8R8A8_UNORM;
		_format.colorSpace = surface_formats[0].colorSpace;
	} else {
		auto found_b8g8r8a8_unorm = false;

		for (const auto& surface_format : surface_formats) {
			if (surface_format.format == VK_FORMAT_B8G8R8A8_UNORM) {
				_format.format = surface_format.format;
				_format.colorSpace = surface_format.colorSpace;
				found_b8g8r8a8_unorm = true;
				break;
			}
		}

		if (!found_b8g8r8a8_unorm) {
			_format.format = surface_formats[0].format;
			_format.colorSpace = surface_formats[0].colorSpace;
		}
	}

	auto present_support = std::uint32_t{0};
	vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, logical_device.present_queue().family, _handle, &present_support);

	if (!present_support) {
		throw std::runtime_error("Present queue family does not have presentation support");
	}
}

surface::~surface() {
  auto& instance = graphics_module::get().instance();

	vkDestroySurfaceKHR(instance, _handle, nullptr);
}

VkSurfaceKHR surface::handle() const noexcept {
  return _handle;
}

surface::operator VkSurfaceKHR() const noexcept {
  return _handle;
}

const VkSurfaceCapabilitiesKHR& surface::capabilities() const noexcept {
  return _capabilities;
}

const VkSurfaceFormatKHR& surface::format() const noexcept {
  return _format;
}

} // namespace sbx::graphics
