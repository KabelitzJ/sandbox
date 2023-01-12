#include <libsbx/graphics/devices/instance.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/devices/extensions.hpp>
#include <libsbx/graphics/devices/validation_layers.hpp>

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
  const auto app_info = VkApplicationInfo{
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = "Demo",
    .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
    .pEngineName = "Sandbox",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_0
  };

  const auto extentions = extensions::instance();
  const auto layers = validation_layers::instance();

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
    .enabledLayerCount = static_cast<std::uint32_t>(layers.size()),
    .ppEnabledLayerNames = layers.data(),
    .enabledExtensionCount = static_cast<std::uint32_t>(extentions.size()),
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

instance::~instance() {
#if defined (SBX_DEBUG)
  _destroy_debug_messenger(_handle, _debug_messenger, nullptr);
#endif
  vkDestroyInstance(_handle, nullptr);
}

auto instance::handle() const noexcept -> VkInstance {
  return _handle;
}

instance::operator VkInstance() const noexcept {
  return _handle;
}

#if defined(SBX_DEBUG)

auto instance::_populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info) -> void {
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = _debug_callback;
}

auto instance::_create_debug_messenger(VkInstance instance_handle, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger) -> VkResult {
  auto function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance_handle, "vkCreateDebugUtilsMessengerEXT"));

  if (function) {
    return function(instance_handle, create_info, allocator, debug_messenger);
  }

  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

auto instance::_destroy_debug_messenger(VkInstance instance_handle, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator) -> void {
  auto function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance_handle, "vkDestroyDebugUtilsMessengerEXT"));

  if (function) {
    function(instance_handle, debug_messenger, allocator);
  }
}

#endif // SBX_DEBUG

} // namespace sbx::graphics
