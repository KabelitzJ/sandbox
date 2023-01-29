#include <libsbx/graphics/devices/instance.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/target.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/devices/extensions.hpp>
#include <libsbx/graphics/devices/validation_layers.hpp>

namespace sbx::graphics {

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

  auto instance_create_info = VkInstanceCreateInfo{};
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pNext = debug_messenger_t::create_info();
  instance_create_info.pApplicationInfo = &app_info;
  instance_create_info.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
  instance_create_info.ppEnabledLayerNames = layers.data();
  instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(extentions.size());
  instance_create_info.ppEnabledExtensionNames = extentions.data();

  validate(vkCreateInstance(&instance_create_info, nullptr, &_handle));

  if constexpr (core::build_configuration_v == core::build_configuration::debug) {
    validate(debug_messenger_t::initialize(*this));
  }
}

instance::~instance() {
  if constexpr (core::build_configuration_v == core::build_configuration::debug) {
    debug_messenger_t::terminate(*this);
  }

  vkDestroyInstance(_handle, nullptr);
}

auto instance::handle() const noexcept -> const VkInstance& {
  return _handle;
}

instance::operator const VkInstance&() const noexcept {
  return _handle;
}

} // namespace sbx::graphics
