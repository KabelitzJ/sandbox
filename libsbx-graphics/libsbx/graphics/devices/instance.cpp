#include <libsbx/graphics/devices/instance.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/graphics_module.hpp>
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
  auto app_info = VkApplicationInfo{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Demo";
  app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
  app_info.pEngineName = "Sandbox";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  const auto extentions = extensions::instance();
  const auto layers = validation_layers::instance();

#if defined(SBX_DEBUG)
  auto instance_debug_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
  _populate_debug_messenger_create_info(instance_debug_messenger_create_info);
#endif

  auto instance_create_info = VkInstanceCreateInfo{};
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if defined(SBX_DEBUG)
  instance_create_info.pNext = &instance_debug_messenger_create_info;
#endif
  instance_create_info.pApplicationInfo = &app_info;
  instance_create_info.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
  instance_create_info.ppEnabledLayerNames = layers.data();
  instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(extentions.size());
  instance_create_info.ppEnabledExtensionNames = extentions.data();

  validate(vkCreateInstance(&instance_create_info, nullptr, &_handle));

#if defined (SBX_DEBUG)
  auto debug_messenger_create_info = VkDebugUtilsMessengerCreateInfoEXT{};
  _populate_debug_messenger_create_info(debug_messenger_create_info);

  validate(_create_debug_messenger(_handle, &debug_messenger_create_info, nullptr, &_debug_messenger));
#endif
}

instance::~instance() {
#if defined (SBX_DEBUG)
  _destroy_debug_messenger(_handle, _debug_messenger, nullptr);
#endif
  vkDestroyInstance(_handle, nullptr);
}

auto instance::handle() const noexcept -> const VkInstance& {
  return _handle;
}

instance::operator const VkInstance&() const noexcept {
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
