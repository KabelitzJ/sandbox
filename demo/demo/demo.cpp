#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <chrono>
#include <memory>
#include <algorithm>
#include <iterator>

#include <libsbx/utility/utility.hpp>
#include <libsbx/units/units.hpp>
#include <libsbx/memory/memory.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/async/async.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/scripting/scripting.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp> 

class demo_application : public sbx::core::application {

public:

  demo_application(sbx::utility::observer_ptr<sbx::core::engine> engine)
  : sbx::core::application{engine} {
    auto& window = sbx::devices::devices_module::get().window();

    window.set_on_window_closed([this]([[maybe_unused]] const sbx::devices::window_closed_event& event){
      _engine->quit();
    });

    window.set_on_key([this]([[maybe_unused]] const sbx::devices::key_event& event){
      if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS) {
        _engine->quit();
      }
    });

    auto& scripting_module = sbx::scripting::scripting_module::get();

    for (const auto& entry : std::filesystem::directory_iterator("./demo/assets/scripts")) {
      if (entry.is_regular_file()) {
        scripting_module.load_script(entry.path());
      }
    }

    auto& graphics_module = sbx::graphics::graphics_module::get();

    for (const auto& entry : std::filesystem::directory_iterator("./demo/assets/shaders")) {
      if (entry.is_directory()) {
        graphics_module.load_pipeline(entry.path());
      }
    }

    window.show();
  }

  ~demo_application() override = default;

  auto update() -> void  {

  }

}; // class demo_application

auto main(int argc, const char** argv) -> int {
  try {
    auto args = std::vector<std::string>{argv, argv + argc};
    auto engine = std::make_unique<sbx::core::engine>(std::move(args));

    engine->run<demo_application>();
  } catch(const std::exception& exception) {
    sbx::core::logger::error("{}", exception.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
