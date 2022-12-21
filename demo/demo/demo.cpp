#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/core/core.hpp>
#include <libsbx/utility/utility.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>

int main() {
  using clock_type = std::chrono::high_resolution_clock;

  sbx::core::logger::debug("libsbx-core:     {}", LIBSBX_CORE_VERSION_STR);
  sbx::core::logger::debug("libsbx-utility:  {}", LIBSBX_UTILITY_VERSION_STR);
  sbx::core::logger::debug("libsbx-math:     {}", LIBSBX_MATH_VERSION_STR);
  sbx::core::logger::debug("libsbx-io:       {}", LIBSBX_IO_VERSION_STR);
  sbx::core::logger::debug("libsbx-ecs:      {}", LIBSBX_ECS_VERSION_STR);
  sbx::core::logger::debug("libsbx-devices:  {}", LIBSBX_DEVICES_VERSION_STR);
  sbx::core::logger::debug("libsbx-graphics: {}", LIBSBX_GRAPHICS_VERSION_STR);

#if defined(LIBSBX_DEBUG)
  auto start = clock_type::now();
#endif

  try {
    sbx::core::module_manager::create_all();
  } catch (const std::exception& exception) {
    sbx::core::logger::error("{}", exception.what());
    return EXIT_FAILURE;
  }

#if defined(LIBSBX_DEBUG)
  auto end = clock_type::now();

  auto duration = std::chrono::duration_cast<std::chrono::duration<std::float_t>>(end - start).count();

  sbx::core::logger::debug("Created all modules successfully (took {}s)", duration);
#endif

  auto& window = sbx::devices::device_module::get().current_window();

  auto on_key_pressed = sbx::core::slot<sbx::devices::key_pressed_event>{[&window](const sbx::devices::key_pressed_event& event){
    if (event.key == sbx::devices::key::escape) {
      window.close();
    } else {
      sbx::core::logger::debug("key pressed: {}", event.key);
    }
  }};

  window.register_on_key_pressed(on_key_pressed);

  auto last = clock_type::now();
  
  while (!window.should_close()) {
    auto now = clock_type::now();
    const auto delta_time = sbx::core::time{now - last};
    last = now;

    sbx::core::module_manager::update_stages(delta_time);
  }

  sbx::core::module_manager::destroy_all();

  return EXIT_SUCCESS;
}
