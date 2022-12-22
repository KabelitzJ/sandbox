#ifndef LIBSBX_GRAPHICS_DEVICES_EXTENTIONS_HPP_
#define LIBSBX_GRAPHICS_DEVICES_EXTENTIONS_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

#include <libsbx/core/platform.hpp>

#include <libsbx/devices/device_module.hpp>

namespace sbx::graphics {

struct extentions {
  static std::vector<const char*> instance() noexcept {
    auto extensions = devices::device_module::get().required_extensions();

    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

#if defined(LIBSBX_DEBUG)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return extensions;
  }

  static std::vector<const char*> device() noexcept {
    return std::vector<const char*>{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
  }
}; // struct instance_extentions

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_EXTENTIONS_HPP_
