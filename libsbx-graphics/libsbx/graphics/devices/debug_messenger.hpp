#ifndef LIBSBX_GRAPHICS_DEVICES_DEBUG_MESSENGER_HPP_
#define LIBSBX_GRAPHICS_DEVICES_DEBUG_MESSENGER_HPP_

#include <vulkan/vulkan.hpp>

#include <libsbx/core/target.hpp>
#include <libsbx/core/logger.hpp>

namespace sbx::graphics {

template<typename Target>
class debug_messenger {

public:

  debug_messenger() = delete;

  ~debug_messenger() {
    static_assert(std::is_same_v<decltype(std::declval<Target>().handle()), const VkInstance&>, "Target type does not fulfill the requirements");
  }

  [[nodiscard]] static auto initialize(const Target& target, const VkAllocationCallbacks* allocator = nullptr) -> VkResult {
    if constexpr (core::build_configuration_v == core::build_configuration::debug) {
      auto* function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(target.handle(), "vkCreateDebugUtilsMessengerEXT"));

      if (function) {
        return function(target.handle(), create_info(), allocator, &_handle);
      }

      return VK_ERROR_EXTENSION_NOT_PRESENT;
    } else {
      return VK_SUCCESS;
    }
  }

  static auto terminate(const Target& target, const VkAllocationCallbacks* allocator = nullptr) -> void {
    if constexpr (core::build_configuration_v == core::build_configuration::debug) {
      auto* function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(target.handle(), "vkDestroyDebugUtilsMessengerEXT"));

      if (function) {
        function(target.handle(), _handle, allocator);
      }
    }
  }

  static auto create_info() -> VkDebugUtilsMessengerCreateInfoEXT* {
    if constexpr (core::build_configuration_v == core::build_configuration::debug) {
      static auto create_info = VkDebugUtilsMessengerCreateInfoEXT{};

      create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
      create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      create_info.pfnUserCallback = _debug_callback;

      return &create_info;
    } else {
      return nullptr;
    }
  }

private:

  static VKAPI_ATTR auto VKAPI_CALL _debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, [[maybe_unused]] void* user_data) -> VkBool32 {
    if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      core::logger::warn("{}", callback_data->pMessage);
    } else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
      core::logger::error("{}", callback_data->pMessage);
    }

    return VK_FALSE;
  }

  inline static VkDebugUtilsMessengerEXT _handle{};

}; // class debug_messenger

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_DEVICES_DEBUG_MESSENGER_HPP_
