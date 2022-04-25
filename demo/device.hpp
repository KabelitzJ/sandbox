#ifndef DEMO_DEVICE_HPP_
#define DEMO_DEVICE_HPP_

#include <map>
#include <optional>
#include <set>
#include <string> 

#include <platform/target.hpp>

#if defined(SBX_PLATFORM_WINDOWS)
  #define VK_USE_PLATFORM_WIN32_KHR
  #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

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

struct queue_family_indices {
  std::optional<sbx::uint32> graphics_family{};
  std::optional<sbx::uint32> present_family{};

  bool is_complete() const noexcept {
    return graphics_family.has_value() && present_family.has_value();
  }
};

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
    _surface{VK_NULL_HANDLE},
    _physical_device{VK_NULL_HANDLE},
    _logical_device{VK_NULL_HANDLE},
    _graphics_queue{VK_NULL_HANDLE},
    _present_queue{VK_NULL_HANDLE} {
    _create_instance();
#if defined(SBX_DEBUG)
    _setup_debug_massager();
#endif
    _create_surface();
    _pick_physical_device();
    _create_logical_device();
  }

  device(const device& other) = delete;

  device(device&& other) noexcept = delete;

  ~device() {
#if defined(SBX_DEBUG)
    destroy_debug_utils_messeger_ext(_instance, _debug_massager, nullptr);
#endif

    vkDestroyDevice(_logical_device, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyInstance(_instance, nullptr);
  }

  device& operator=(const device& other) = delete;

  device& operator=(device&& other) noexcept = delete;

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
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
          log->info("[Vulkan Error] {0}", callback_data->pMessage);
          break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
          log->warn("[Vulkan Error] {0}", std::string{callback_data->pMessage});
          break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
          log->error("[Vulkan Error] {0}", std::string{callback_data->pMessage});
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
    auto physical_device_count = sbx::uint32{};
    vkEnumeratePhysicalDevices(_instance, &physical_device_count, nullptr);

    if (physical_device_count == 0) {
      throw std::runtime_error("Failed to find GPUs with vulkan support!");
    }

    auto physical_devices = std::vector<VkPhysicalDevice>(physical_device_count);
    vkEnumeratePhysicalDevices(_instance, &physical_device_count, physical_devices.data());

    auto candidates = std::multimap<sbx::uint32, VkPhysicalDevice>();

    for (const auto& physical_device : physical_devices) {
      const auto rating = _rate_physical_device(physical_device);
      candidates.insert(std::make_pair(rating, physical_device));
    }

    if (candidates.crbegin()->first == 0) {
      throw std::runtime_error("Failed to find a suitable GPU!");
    }

    _physical_device = candidates.crbegin()->second;

#if defined(SBX_DEBUG)
    _log_physical_device(_physical_device);
#endif
  }

  sbx::uint32 _rate_physical_device(VkPhysicalDevice physical_device) {
    auto score = sbx::uint32{0};

    auto queue_families = _find_queue_families(physical_device);

    if (!queue_families.is_complete()) {
      return 0;
    }

    if (!_check_physical_device_extentions_support(physical_device)) {
      return 0;
    }

    auto properties = VkPhysicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    auto features = VkPhysicalDeviceFeatures{};
    vkGetPhysicalDeviceFeatures(physical_device, &features);

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }

    // [TODO] KAJ 2022-04-25 16:54 - Add other criteria here

    score += properties.limits.maxImageDimension2D;

    return score;
  }

  bool _check_physical_device_extentions_support(VkPhysicalDevice physical_device) {
    auto available_extention_count = sbx::uint32{0};
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &available_extention_count, nullptr);

    auto available_extentions = std::vector<VkExtensionProperties>{available_extention_count};
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &available_extention_count, available_extentions.data());

    auto extentions = _device_extentions();

    auto required_extentions = std::set<std::string>{extentions.begin(), extentions.end()};

    for (const auto& extention : available_extentions) {
      required_extentions.erase(extention.extensionName);
    }

    return required_extentions.empty();
  }

  std::vector<const char*> _device_extentions() {
    return {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
  }

  void _log_physical_device(VkPhysicalDevice physical_device) {
    auto properties = VkPhysicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    _logger->info("Selected {0}", properties.deviceName);
  }

  queue_family_indices _find_queue_families(VkPhysicalDevice physical_device) {
    auto indices = queue_family_indices{};

    auto queue_family_count = sbx::uint32{0};
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    auto queue_families = std::vector<VkQueueFamilyProperties>(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    auto index = sbx::uint32{0};

    for (const auto& queue_family : queue_families) {
      if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphics_family = index;
      }

      if (_surface == VK_NULL_HANDLE) {
        continue;
      }

      auto has_present_support = VkBool32{false};
      vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, index, _surface, &has_present_support);

      if (has_present_support) {
        indices.present_family = index;
      }

      if (indices.is_complete()) {
        break;
      }

      ++index;
    }

    return indices;
  }

  void _create_logical_device() {
    auto queue_families = _find_queue_families(_physical_device);

    auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};
    auto unique_queue_families = std::set<sbx::uint32>{
      queue_families.graphics_family.value(),
      queue_families.present_family.value()
    };

    const auto queue_priority = 1.0f;

    for (const auto queue_family : unique_queue_families) {
      const auto queue_create_info = VkDeviceQueueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queue_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority
      };

      queue_create_infos.push_back(queue_create_info);
    }

    const auto device_features = VkPhysicalDeviceFeatures{};

    const auto create_info = VkDeviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCreateInfoCount = static_cast<sbx::uint32>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),
      .enabledLayerCount = _validation_layers.count(),
      .ppEnabledLayerNames = _validation_layers.names(),
      .enabledExtensionCount = static_cast<sbx::uint32>(_device_extentions().size()),
      .ppEnabledExtensionNames = _device_extentions().data(),
      .pEnabledFeatures = &device_features
    };

    if (vkCreateDevice(_physical_device, &create_info, nullptr, &_logical_device) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create logical device!");
    }

    vkGetDeviceQueue(_logical_device, queue_families.graphics_family.value(), 0, &_graphics_queue);
    vkGetDeviceQueue(_logical_device, queue_families.present_family.value(), 0, &_present_queue);
  }

  void _create_surface() {
    if (glfwCreateWindowSurface(_instance, _window->native_handle(), nullptr, &_surface) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create window surface!");
    }
  }

  logger* _logger{};
  configuration* _configuration{};
  window* _window{};

  validation_layers _validation_layers{};
  extentions _extentions{};

  VkInstance _instance{};
  VkDebugUtilsMessengerEXT _debug_massager{};
  VkSurfaceKHR _surface{};
  VkPhysicalDevice _physical_device{};
  VkDevice _logical_device{};
  VkQueue _graphics_queue{};
  VkQueue _present_queue{};


}; // class device

} // namespace demo

#endif // DEMO_DEVICE_HPP_
