#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/utility/utility.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/scripting/scripting.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>

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
    auto& window = sbx::devices::devices_module::get().window();

    auto v = sbx::math::vector3{};

    sbx::core::logger::info("{}", v);

    auto output = sbx::io::node{};

    auto d = data{ 
      .integer = 42, 
      .floating_point = 0.69f,
      .transform = transform{
        .position = sbx::math::vector3{1.0f, 0.0f, 0.0f},
        .rotation = sbx::math::vector3{0.0f, 1.0f, 0.0f},
        .scale = sbx::math::vector3{0.0f, 0.0f, 1.0f}
      }
    };

    output["data"] = d;
    output["list"] = sbx::io::node::list_type{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    output["map"] = sbx::io::node::map_type{{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}};


    auto output_file = std::ofstream{"./demo/assets/data/data.sbx"};

    if (output_file.is_open()) {
      output_file << output.to_string();
      output_file.close();
    }

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
