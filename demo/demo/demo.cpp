#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/core/core.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>

class demo_application : public sbx::core::application {

public:

  demo_application(sbx::core::engine& engine) {
    auto& window = sbx::devices::device_module::get().window();

    window.set_on_window_closed([&engine]([[maybe_unused]] const sbx::devices::window_closed_event& event){
      engine.quit();
    });
  }

  ~demo_application() override = default;

  auto update() -> void  {
    
  }

}; // class demo_application

auto main(int argc, char** argv) -> int {
  try {
    auto engine = std::make_unique<sbx::core::engine>(std::vector<std::string_view>{argv, argv + argc});
    auto application = std::make_unique<demo_application>(*engine);
    engine->run(application);
  } catch(const std::exception& exception) {
    sbx::core::logger::error("{}", exception.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
