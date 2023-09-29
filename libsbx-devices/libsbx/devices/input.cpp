#include <libsbx/devices/input.hpp>

#include <libsbx/devices/devices_module.hpp>

namespace sbx::devices {

auto input::is_key_pressed(key key) -> bool {
  auto& devices_module = core::engine::get_module<devices::devices_module>();
  auto& window = devices_module.window();

  const auto input_action = static_cast<devices::input_action>(glfwGetKey(window, static_cast<std::int32_t>(key)));

  return input_action == devices::input_action::press;
}

auto input::is_key_released(key key) -> bool {
  auto& devices_module = core::engine::get_module<devices::devices_module>();
  auto& window = devices_module.window();

  const auto input_action = static_cast<devices::input_action>(glfwGetKey(window, static_cast<std::int32_t>(key)));

  return input_action == devices::input_action::release;
}

} // namespace sbx::devices
