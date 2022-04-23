#ifndef DEMO_DEVICE_HPP_
#define DEMO_DEVICE_HPP_

#include <string>

#include <GLFW/glfw3.h>

#include "validation_layers.hpp"
#include "extentions.hpp"
#include "configuration.hpp"
#include "window.hpp"
#include "logger.hpp"

namespace demo {

// [TODO] KAJ 2022-04-23 17:17 - Refactor debugging out

VkResult create_debug_utils_messenger_ext(
  VkInstance instance, 
  const VkDebugUtilsMessengerCreateInfoEXT* create_info, 
  const VkAllocationCallbacks* allocator, 
  VkDebugUtilsMessengerEXT* debug_messenger
) {
  auto function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

  if (function != nullptr) {
    return function(instance, create_info, allocator, debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroy_debug_utils_messeger_ext(
  VkInstance instance,
  VkDebugUtilsMessengerEXT debug_messenger, 
  const VkAllocationCallbacks* allocator
) {
  auto function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

  if (function != nullptr) {
    function(instance, debug_messenger, allocator);
  }
}

class device {

public:

  device(logger* logger, configuration* configuration, window* window)
  : _logger{logger},
    _configuration{configuration},
    _window{window},
    _validation_layers{},
    _extentions{},
    _instance{VK_NULL_HANDLE},
    _debug_massager{VK_NULL_HANDLE},
    _physical_device{VK_NULL_HANDLE},
    _logical_device{VK_NULL_HANDLE} {
    _create_instance();
#ifndef NDEBUG
    _setup_debug_massager();
#endif
    _pick_physical_device();
    _create_logical_device();
  }

  device(const device& other) = delete;

  device(device&& other) noexcept;

  ~device() {
#ifndef NDEBUG
    destroy_debug_utils_messeger_ext(_instance, _debug_massager, nullptr);
#endif

    vkDestroyInstance(_instance, nullptr);
  }

  device& operator=(const device& other) = delete;

  device& operator=(device&& other) noexcept;

private:

  static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data
  ) {
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      auto* log = static_cast<logger*>(user_data);
      
      switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
          log->warn("{0}", std::string{callback_data->pMessage});
          break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
          log->error("{0}", std::string{callback_data->pMessage});
          break;
        default:
          break;
      }
    }

    return VK_FALSE;
  }

  void _create_instance() {
    const auto app_info = VkApplicationInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = _configuration->get<std::string>("name").c_str(),
      .applicationVersion = VK_MAKE_VERSION(
        _configuration->get<sbx::uint32>("version.major"),
        _configuration->get<sbx::uint32>("version.minor"),
        _configuration->get<sbx::uint32>("version.patch")
      ),
      .pEngineName = "Sandbox Engine",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),
      .apiVersion = VK_API_VERSION_1_0
    };

#ifndef NDEBUG
    auto debug_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
    _populate_debug_messenger_create_info(debug_create_info);
#endif

    const auto create_info = VkInstanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifndef NDEBUG
      .pNext = &debug_create_info,
#else
      .pNext = nullptr,
#endif
      .flags = 0,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = _validation_layers.count(),
      .ppEnabledLayerNames = _validation_layers.names(),
      .enabledExtensionCount = _extentions.count(),
      .ppEnabledExtensionNames = _extentions.names()
    };

    if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create vulkan instance!");
    }
  }

  void _setup_debug_massager() {
    auto create_info = VkDebugUtilsMessengerCreateInfoEXT{};
    _populate_debug_messenger_create_info(create_info);

    if (create_debug_utils_messenger_ext(_instance, &create_info, nullptr, &_debug_massager) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }

  void _populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
    create_info = VkDebugUtilsMessengerCreateInfoEXT{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = _debug_callback,
      .pUserData = _logger
    };
  }

  void _pick_physical_device() {

  }

  void _create_logical_device() {

  }

  logger* _logger{};
  configuration* _configuration{};
  window* _window{};

  validation_layers _validation_layers{};
  extentions _extentions{};

  VkInstance _instance{};
  VkDebugUtilsMessengerEXT _debug_massager{};
  VkPhysicalDevice _physical_device{};
  VkDevice _logical_device{};


}; // class device

} // namespace demo

#endif // DEMO_DEVICE_HPP_
