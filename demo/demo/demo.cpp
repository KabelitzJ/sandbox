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

    for (const auto& entry : std::filesystem::directory_iterator("./demo/assets/scripts")) {
      if (entry.is_regular_file()) {
        scripting_module.load_script(entry.path());
      }
    }

    auto& main_script = scripting_module.script("main");

    main_script.set("position", sbx::math::vector3f{1.0f, 2.0f, 3.0f});
    main_script.set("velocity", sbx::math::vector3f{4.0f, 5.0f, 6.0f});
    main_script.set("acceleration", sbx::math::vector3f{7.0f, 8.0f, 9.0f});
    main_script.startup();
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
