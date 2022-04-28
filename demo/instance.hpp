#ifndef DEMO_INSTANCE_HPP_
#define DEMO_INSTANCE_HPP_

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <platform/target.hpp>

#include "logger.hpp"
#include "configuration.hpp"
#include "validation_layers.hpp"

namespace demo {

class instance {

public:

  instance(logger* logger, configuration* configuration)
  : _logger{logger},
    _configuration{configuration},
    _instance{VK_NULL_HANDLE},
    _debug_massager{VK_NULL_HANDLE} {
    _create_instance();

#if defined(SBX_DEBUG)
    _create_debug_massager();
#endif
  }

  ~instance() {
#if defined(SBX_DEBUG)
    _destroy_debug_utils_messeger_ext(_instance, _debug_massager, nullptr);
#endif

    vkDestroyInstance(_instance, nullptr);
  }

private:

  static VkResult _create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger) {
    auto function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (function != nullptr) {
      return function(instance, create_info, allocator, debug_messenger);
    } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  static void _destroy_debug_utils_messeger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator) {
    auto function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (function != nullptr) {
      function(instance, debug_messenger, allocator);
    }
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      auto* log = static_cast<logger*>(user_data);
      
      switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
          log->info("{0}", callback_data->pMessage);
          break;
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

    auto extentions = _extentions();

#if defined(SBX_DEBUG)
    auto debug_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
    _populate_debug_messenger_create_info(debug_create_info);
#endif

    const auto create_info = VkInstanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if defined(SBX_DEBUG)
      .pNext = &debug_create_info,
#else
      .pNext = nullptr,
#endif
      .flags = 0,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = validation_layers::count(),
      .ppEnabledLayerNames = validation_layers::names(),
      .enabledExtensionCount = static_cast<sbx::uint32>(extentions.size()),
      .ppEnabledExtensionNames = extentions.data()
    };

    if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create vulkan instance!");
    }
  }

  std::vector<const char*> _extentions() {
    auto glfw_extention_count = sbx::uint32{0};
    const auto** glfw_extentions = glfwGetRequiredInstanceExtensions(&glfw_extention_count);

    auto required_extentions = std::vector<const char*>{glfw_extentions, glfw_extentions + glfw_extention_count};

#if defined(SBX_DEBUG)
    required_extentions.reserve(required_extentions.size() + _validation_extentions.size());
    required_extentions.insert(required_extentions.end(), _validation_extentions.begin(), _validation_extentions.end());
#endif

    auto extention_count = sbx::uint32{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extention_count, nullptr);

    auto available_extentions = std::vector<VkExtensionProperties>(extention_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extention_count, available_extentions.data());

    for (const auto& extention : required_extentions) {
      auto found = false;

      for (const auto& available_extention : available_extentions) {
        if (std::strcmp(available_extention.extensionName, extention) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        throw std::runtime_error("Failed to find required instance extention!");
      }
    }

    return required_extentions;
  }


#if defined(SBX_DEBUG)
  void _create_debug_massager() {
    auto create_info = VkDebugUtilsMessengerCreateInfoEXT{};
    _populate_debug_messenger_create_info(create_info);

    if (_create_debug_utils_messenger_ext(_instance, &create_info, nullptr, &_debug_massager) != VK_SUCCESS) {
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
#endif

  logger* _logger{};
  configuration* _configuration{};

  VkInstance _instance{};

#if defined(SBX_DEBUG)
  inline static const std::vector<const char*> _validation_extentions{
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
  };

  VkDebugUtilsMessengerEXT _debug_massager{};
#endif

}; // class instance

} // namespace demo

#endif // DEMO_INSTANCE_HPP_
