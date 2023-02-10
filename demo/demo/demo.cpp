#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <chrono>

#include <libsbx/utility/utility.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/async/async.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/scripting/scripting.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>

#include <demo/quantity.hpp>
#include <demo/distance.hpp>
#include <demo/mass.hpp>
#include <demo/time.hpp>
#include <demo/velocity.hpp>

class demo_application : public sbx::core::application {

public:

  demo_application(sbx::core::engine& engine)
  : _engine{engine} {
    auto& window = sbx::devices::devices_module::get().window();

    window.set_on_window_closed([this]([[maybe_unused]] const sbx::devices::window_closed_event& event){
      _engine.quit();
    });

    auto& scripting_module = sbx::scripting::scripting_module::get();

    scripting_module.load_script("./demo/assets/scripts/test.lua");
    scripting_module.load_script("./demo/assets/scripts/main.lua");
  }

  ~demo_application() override = default;

  auto update() -> void  {

  }

private:

  sbx::core::engine& _engine;

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
