#ifndef DEMO_INSTANCE_HPP_
#define DEMO_INSTANCE_HPP_

#include <vector>

#include <vulkan/vulkan.hpp>

#include <types/primitives.hpp>
#include <platform/target.hpp>
#include <utils/noncopyable.hpp>

#include "logger.hpp"
#include "window.hpp"
#include "configuration.hpp"

namespace demo {

class instance : sbx::noncopyable {

  friend class logical_device;

public:

  instance(logger* logger, window* window, configuration* configuration)
  : _logger{logger},
    _window{window},
    _configuration{configuration},
#if defined(SBX_DEBUG)
    _debug_messenger{nullptr},
#endif
    _handle{nullptr} {
    _initialize();
  }

  ~instance() {
    _terminate();
  }

  [[nodiscard]] VkInstance handle() const noexcept {
    return _handle;
  }

private:

  static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data
  ) {
    auto log = static_cast<logger*>(user_data);

    if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      log->warn("{}", callback_data->pMessage);
    } else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
      log->error("{}", callback_data->pMessage);
    }

    return VK_FALSE;
  }

  void _initialize() {
    const auto& name = _configuration->app_name();
    const auto& version = _configuration->app_version();

    const auto app_info = VkApplicationInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = name.c_str(),
      .applicationVersion = VK_MAKE_VERSION(version.major, version.minor, version.patch),
      .pEngineName = "Sandbox",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0
    };

    const auto extentions = _extensions();
    const auto layers = _layers();

#if defined(SBX_DEBUG)
    auto instance_debug_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
    _populate_debug_messenger_create_info(instance_debug_messenger_create_info);
#endif

    const auto instance_create_info = VkInstanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if defined(SBX_DEBUG)
      .pNext = &instance_debug_messenger_create_info,
#else
      .pNext = nullptr,
#endif
      .flags = 0,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = static_cast<sbx::uint32>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<sbx::uint32>(extentions.size()),
      .ppEnabledExtensionNames = extentions.data()
    };

    if (vkCreateInstance(&instance_create_info, nullptr, &_handle) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create instance"};
    }

#if defined (SBX_DEBUG)
    auto debug_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
    _populate_debug_messenger_create_info(debug_messenger_create_info);

    if (_create_debug_messenger(_handle, &debug_messenger_create_info, nullptr, &_debug_messenger) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create debug messenger"};
    }
#endif
  }

  void _terminate() {
#if defined (SBX_DEBUG)
    _destroy_debug_messenger(_handle, _debug_messenger, nullptr);
#endif
    vkDestroyInstance(_handle, nullptr);
  }

  std::vector<const char*> _extensions() {
    const auto required_extensions = _window->_required_extensions();

    auto available_extention_count = sbx::uint32{0};
    auto available_extensions = std::vector<VkExtensionProperties>{};

    vkEnumerateInstanceExtensionProperties(nullptr, &available_extention_count, nullptr);

    available_extensions.resize(available_extention_count);

    vkEnumerateInstanceExtensionProperties(nullptr, &available_extention_count, available_extensions.data());

    for (const auto* required_extension : required_extensions) {
      bool found = false;

      for (const auto& available_extension : available_extensions) {
        if (std::strcmp(required_extension, available_extension.extensionName) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        throw std::runtime_error{"Required extension not available: " + std::string{required_extension}};
      }
    }

    return required_extensions;
  }

  std::vector<const char*> _layers() {
#if defined(SBX_DEBUG)
    const auto required_layers = {
      "VK_LAYER_KHRONOS_validation"
    };

    auto available_layer_count = sbx::uint32{0};
    auto available_layers = std::vector<VkLayerProperties>{};

    vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);

    available_layers.resize(available_layer_count);

    vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data());

    for (const auto* required_layer : required_layers) {
      bool found = false;

      for (const auto& available_layer : available_layers) {
        if (std::strcmp(required_layer, available_layer.layerName) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        throw std::runtime_error{"Required layer not available: " + std::string{required_layer}};
      }
    }

    return required_layers;
#else
    return std::vector<const char*>{};
#endif
  }

#if defined(SBX_DEBUG)
  void _populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
    create_info = VkDebugUtilsMessengerCreateInfoEXT{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = _debug_callback,
      .pUserData = _logger
    };
  }

  VkResult _create_debug_messenger(
    VkInstance instance_handle, 
    const VkDebugUtilsMessengerCreateInfoEXT* create_info, 
    const VkAllocationCallbacks* allocator, 
    VkDebugUtilsMessengerEXT* debug_messenger
  ) {
    auto function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance_handle, "vkCreateDebugUtilsMessengerEXT"));

    if (function) {
      return function(instance_handle, create_info, allocator, debug_messenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  void _destroy_debug_messenger(
    VkInstance instance_handle,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks* allocator
  ) {
    auto function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance_handle, "vkDestroyDebugUtilsMessengerEXT"));

    if (function) {
      function(instance_handle, debug_messenger, allocator);
    }
  }
#endif

  logger* _logger{};
  window* _window{};
  configuration* _configuration{};

#if defined(SBX_DEBUG)
  VkDebugUtilsMessengerEXT _debug_messenger{};
#endif

  VkInstance _handle{};

}; // class instance

} // namespace demo

#endif // DEMO_INSTANCE_HPP_
