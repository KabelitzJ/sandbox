#ifndef LIBSBX_GRAPHICS_DEVICES_EXTENSIONS_HPP_
#define LIBSBX_GRAPHICS_DEVICES_EXTENSIONS_HPP_

#include <vector>

#include <fmt/format.h>

#include <vulkan/vulkan.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/utility/target.hpp>

#include <libsbx/devices/devices_module.hpp>

namespace sbx::graphics {

struct extensions {

  static auto device() -> std::vector<const char*> {
    return std::vector<const char*>{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME
    };
  }

  static auto instance() -> std::vector<const char*> {
    auto& devices_module = core::engine::get_module<devices::devices_module>();

    auto required_extensions = devices_module.required_instance_extensions();

    required_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    auto available_extention_count = std::uint32_t{0};
    auto available_extensions = std::vector<VkExtensionProperties>{};

    vkEnumerateInstanceExtensionProperties(nullptr, &available_extention_count, nullptr);

    available_extensions.resize(available_extention_count);

    vkEnumerateInstanceExtensionProperties(nullptr, &available_extention_count, available_extensions.data());

    for (const auto* extension : required_extensions) {
      bool found = false;

      for (const auto& available_extension : available_extensions) {
        if (std::strcmp(extension, available_extension.extensionName) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        throw std::runtime_error{fmt::format("Required extension not available: {}", extension)};
      }
    }

    return required_extensions;
  }

}; // struct extensions

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_EXTENSIONS_HPP_
