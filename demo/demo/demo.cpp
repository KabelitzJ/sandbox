#include <iostream>
#include <cstdlib>
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

struct transform {
  sbx::math::vector3 position{};
  sbx::math::vector3 rotation{};
  sbx::math::vector3 scale{};
};

sbx::io::node& operator<<(sbx::io::node& node, const transform& transform) {
  node["position"] = transform.position;
  node["rotation"] = transform.rotation;
  node["scale"] = transform.scale;

  return node;
}

struct data {
  std::int32_t integer{};
  std::float_t floating_point{};
  transform transform{};
};

sbx::io::node& operator<<(sbx::io::node& node, const data& data) {
  node["integer"] = data.integer;
  node["floating_point"] = data.floating_point;
  node["transform"] = data.transform;

  return node;
}

class demo_application : public sbx::core::application {

public:

  demo_application(sbx::core::engine& engine)
  : _engine{engine} {
    using namespace demo::literals;

    auto& window = sbx::devices::devices_module::get().window();

    window.set_on_window_closed([this]([[maybe_unused]] const sbx::devices::window_closed_event& event){
      _engine.quit();
    });

    auto distance = 1.0_km + 1.0_m + 1.0_dm + 1.0_cm + 1.0_mm;
    auto mass = 1.0_kg + 1.0_g + 1.0_mg;
    auto time = 1.0_s + 1.0_ms + 1.0_us + 1.0_ns;

    sbx::core::logger::info("{} km", distance.value());
    sbx::core::logger::info("{} kg", mass.value());
    sbx::core::logger::info("{} s", time.value());
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
