#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/utility/utility.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>

class demo_application : public sbx::core::application {

public:

  demo_application(sbx::core::engine& engine)
  : _engine{engine} {
    auto& window = sbx::devices::devices_module::get().window();

    auto v = sbx::math::vector3{};

    sbx::core::logger::info("{}", v);

    auto root = sbx::io::node{};

    root["value1"] = 32;
    root["value2"] = false;
    root["value3"] = 11.4f;

    auto& value3 = root["value3"];

    value3["x"] = 1;
    value3["y"] = 2;
    value3["z"] = 3;

    std::cout << root;

    // auto file = std::ofstream{"./demo/assets/io.txt"};

    // if (file.is_open()) {
    //   file << root;
    //   file.close();
    // }

    window.set_on_window_closed([this]([[maybe_unused]] const sbx::devices::window_closed_event& event){
      _engine.quit();
    });
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
