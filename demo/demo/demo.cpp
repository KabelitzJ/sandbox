#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/utility/utility.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>

struct junk {
  std::string value{};
};

struct data {
  std::uint32_t x{};
  std::uint32_t y{};
  std::uint32_t z{};
};

sbx::io::node& operator<<(sbx::io::node& node, const data& data) {
  node["x"] = data.x;
  node["y"] = data.y;
  node["z"] = data.z;

  return node;
}

class demo_application : public sbx::core::application {

public:

  demo_application(sbx::core::engine& engine)
  : _engine{engine} {
    auto& window = sbx::devices::devices_module::get().window();

    auto v = sbx::math::vector3{};

    sbx::core::logger::info("{}", v);

    auto root = sbx::io::node{};

    auto d = data{ .x = 1, .y = 32, .z = 444 };

    root["int"] = 32;
    root["float"] = 11.4f;
    root["bool"] = false;
    root["string"] = "Jonas";
    root["list"] = sbx::io::node::list_type{ 1, 2, 3, 4, 5 };
    root["map"] = sbx::io::node::map_type{{ "Jonas", 22 }, { "Caitlin", 20}};
    root["data"] = d;
    root["position"] = v;

    auto file = std::ofstream{"./demo/assets/io.txt"};

    if (file.is_open()) {
      file << root;
      file.close();
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
