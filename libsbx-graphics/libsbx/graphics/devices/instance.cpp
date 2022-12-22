#include <libsbx/graphics/devices/instance.hpp>

#include <cinttypes>
#include <iostream>

#include <vulkan/vulkan.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/devices/extentions.hpp>
#include <libsbx/graphics/devices/layers.hpp>

namespace sbx::graphics {

static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, [[maybe_unused]] void* user_data) {
  if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    core::logger::warn("{}", callback_data->pMessage);
  } else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    core::logger::error("{}", callback_data->pMessage);
  }

  return VK_FALSE;
}

instance::instance() {
  auto app_info = VkApplicationInfo{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Demo";
  app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
  app_info.pEngineName = "Sandbox";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  const auto extentions = _extensions();

  core::logger::debug("Extentions: {}", fmt::join(extentions, ", "));

  const auto layers = _layers();

  core::logger::debug("Layers: {}", fmt::join(layers, ", "));

  auto instance_create_info = VkInstanceCreateInfo{};
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if defined(LIBSBX_DEBUG)
  auto instance_debug_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
  _populate_debug_messenger_create_info(instance_debug_messenger_create_info);
  instance_create_info.pNext = &instance_debug_messenger_create_info;
#endif
  instance_create_info.pApplicationInfo = &app_info;
  instance_create_info.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
  instance_create_info.ppEnabledLayerNames = layers.data();
  instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(extentions.size());
  instance_create_info.ppEnabledExtensionNames = extentions.data();

  graphics_module::validate(vkCreateInstance(&instance_create_info, nullptr, &_handle));

#if defined(LIBSBX_DEBUG)
  auto debug_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
  _populate_debug_messenger_create_info(debug_messenger_create_info);

  graphics_module::validate(_create_debug_messenger(_handle, &debug_messenger_create_info, nullptr, &_debug_messenger));
#endif
}

instance::~instance() {
#if defined(LIBSBX_DEBUG)
  _destroy_debug_messenger(_handle, _debug_messenger, nullptr);
#endif
  vkDestroyInstance(_handle, nullptr);
}

VkInstance instance::handle() const noexcept {
  return _handle;
}

instance::operator VkInstance() const noexcept {
  return _handle;
}

std::vector<const char*> instance::_extensions() {
  const auto required_extensions = extentions::instance();

  auto available_extention_count = std::uint32_t{0};
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extention_count, nullptr);
  auto available_extensions = std::vector<VkExtensionProperties>{available_extention_count};
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
      throw std::runtime_error{fmt::format("Required extension not available: {}", required_extension)};
    }
  }

  return required_extensions;
}

std::vector<const char*> instance::_layers() {
  const auto required_layers = layers::validation();

  auto available_layer_count = std::uint32_t{0};
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
      throw std::runtime_error{fmt::format("Required layer not available: {}", required_layer)};
    }
  }

  return required_layers;
}

#if defined(LIBSBX_DEBUG)

void instance::_populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
  create_info = VkDebugUtilsMessengerCreateInfoEXT{
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = nullptr,
    .flags = 0,
    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = _debug_callback,
    .pUserData = nullptr
  };
}

VkResult instance::_create_debug_messenger(VkInstance instance_handle, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger) {
  auto function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance_handle, "vkCreateDebugUtilsMessengerEXT"));

  if (function) {
    return function(instance_handle, create_info, allocator, debug_messenger);
  }

  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void instance::_destroy_debug_messenger(VkInstance instance_handle, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator) {
  auto function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance_handle, "vkDestroyDebugUtilsMessengerEXT"));

  if (function) {
    function(instance_handle, debug_messenger, allocator);
  }
}

#endif

} // namespace sbx::graphics
