#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/core/core.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>

class test_module : public sbx::core::module<test_module> {
  
  inline static const auto registered = register_module(stage::normal);

public:

  test_module() = default;
  ~test_module() override = default;

  void update() override {
    
  }

  sbx::ecs::registry& registry() {
    return _registry;
  }

private:

  sbx::ecs::registry _registry{};

}; // class test_module

int main() {
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

  auto last = std::chrono::high_resolution_clock::now();
  
  while (!window.should_close()) {
    auto now = std::chrono::high_resolution_clock::now();
    const auto delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(now - last);
    last = now;

    sbx::core::module_manager::update_stages();

    window.set_title(fmt::format("Window [Delta: {} ms]", delta_time.count()));
  }

  sbx::core::module_manager::destroy_all();

  return EXIT_SUCCESS;
}
