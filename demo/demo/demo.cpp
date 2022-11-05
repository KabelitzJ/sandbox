#include <iostream>
#include <cstdlib>

#include <libsbx/core/core.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/devices/devices.hpp>

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
  std::cout << "libsbx-core: " << LIBSBX_CORE_VERSION_STR << "\n";
  std::cout << "libsbx-ecs: " << LIBSBX_ECS_VERSION_STR << "\n";
  std::cout << "libsbx-devices: " << LIBSBX_DEVICES_VERSION_STR << "\n";

  try {
    sbx::core::module_manager::create_all();
  } catch (const std::exception& exception) {
    sbx::core::logger::error("{}", exception.what());
    return EXIT_FAILURE;
  }
  
  while (!sbx::devices::device_module::get().current_window().should_close()) {
    sbx::core::module_manager::update_stages();
  }

  sbx::core::module_manager::destroy_all();

  return EXIT_SUCCESS;
}
