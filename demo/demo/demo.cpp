#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/core/core.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>

int main() {
  using clock_type = std::chrono::high_resolution_clock;

  sbx::core::logger::info("libsbx-core: {}", LIBSBX_CORE_VERSION_STR);
  sbx::core::logger::info("libsbx-ecs: {}", LIBSBX_ECS_VERSION_STR);
  sbx::core::logger::info("libsbx-devices: {}", LIBSBX_DEVICES_VERSION_STR);
  sbx::core::logger::info("libsbx-graphics: {}", LIBSBX_GRAPHICS_VERSION_STR);

  try {
    sbx::core::module_manager::create_all();
  } catch (const std::exception& exception) {
    sbx::core::logger::error("{}", exception.what());
    return EXIT_FAILURE;
  }


  auto& window = sbx::devices::device_module::get().current_window();

  auto key_pressed_listener = sbx::core::slot<sbx::devices::key_pressed_event>{[&window](const sbx::devices::key_pressed_event& event){
    if (event.key == sbx::devices::key::escape) {
      window.close();
    }
  }};

  sbx::core::core_module::get().dispatcher().connect(key_pressed_listener);

  auto last = clock_type::now();
  
  while (!window.should_close()) {
    auto now = clock_type::now();
    const auto delta_time = sbx::core::time{now - last};
    last = now;

    sbx::core::module_manager::update_stages(delta_time);
  }

  sbx::core::core_module::get().dispatcher().disconnect(key_pressed_listener);

  sbx::core::module_manager::destroy_all();

  return EXIT_SUCCESS;
}
