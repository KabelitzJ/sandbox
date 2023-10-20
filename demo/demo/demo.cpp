#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <chrono>
#include <memory>
#include <algorithm>
#include <iterator>
#include <utility>

#include <libsbx/units/units.hpp>
#include <libsbx/utility/utility.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/memory/memory.hpp>
#include <libsbx/signals/signals.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/async/async.hpp>
#include <libsbx/assets/assets.hpp>
#include <libsbx/audio/audio.hpp>
// #include <libsbx/devices/devices.hpp>
// #include <libsbx/graphics/graphics.hpp>
// #include <libsbx/models/models.hpp>
// #include <libsbx/scenes/scenes.hpp>
// #include <libsbx/ui/ui.hpp>

// class demo_renderer : public sbx::graphics::renderer {

// public:

//   demo_renderer() { 
//     auto render_pass_attachments_1 = std::vector<sbx::graphics::attachment>{
//       sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, VK_FORMAT_UNDEFINED},
//       sbx::graphics::attachment{1, "depth", sbx::graphics::attachment::type::depth}
//     };

//     auto render_pass_subpass_bindings_1 = std::vector<sbx::graphics::subpass_binding>{
//       sbx::graphics::subpass_binding{0, {0, 1}},
//       sbx::graphics::subpass_binding{1, {0}}
//     };

//     add_render_stage(std::move(render_pass_attachments_1), std::move(render_pass_subpass_bindings_1));
//   }

//   ~demo_renderer() override {

//   }

//   auto initialize() -> void override {
//     add_subrenderer<sbx::scenes::scene_subrenderer>("res://shaders/cell_shading", sbx::graphics::pipeline::stage{0, 0});
//     add_subrenderer<sbx::ui::ui_subrenderer>("res://shaders/ui", sbx::graphics::pipeline::stage{0, 1});
//   }

// }; // class demo_renderer

// class demo_application : public sbx::core::application {

// public:

//   demo_application() {
//     auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();
//     auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();
//     auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();
//     auto& ui_module = sbx::core::engine::get_module<sbx::ui::ui_module>();

//     assets_module.set_asset_directory("./demo/assets");

//     auto& window = devices_module.window();

//     window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
//       sbx::core::engine::quit();
//     };

//     graphics_module.set_renderer<demo_renderer>();

//     auto base_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/base.png");
//     auto default_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/default.png");
//     auto grid_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/grid.png");

//     auto monkey_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/suzanne.obj");
//     auto sphere_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/sphere.obj");
//     auto cube_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/cube.obj");

//     auto font_jet_brains_mono_id = assets_module.load_asset<sbx::ui::font>("res://fonts/JetBrainsMono-Medium.ttf", sbx::ui::pixels{48u});
//     auto font_roboto_id = assets_module.load_asset<sbx::ui::font>("res://fonts/Roboto-Regular.ttf", sbx::ui::pixels{48u});

//     ui_module.add_widget<sbx::ui::label>("Hello, World!", sbx::math::vector2u{25, 25}, font_roboto_id, sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f});

//     auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

//     auto& scene = scenes_module.scene();

//     auto monkey = scene.create_node("Monkey");
//     monkey.add_component<sbx::scenes::static_mesh>(monkey_id, base_id);
//     auto& monkey_rotation = monkey.add_component<sbx::scenes::script>("res://scripts/rotate.lua");
//     monkey_rotation.set("speed", 75.0f);

//     auto camera = scene.camera();

//     auto& camera_movement = camera.add_component<sbx::scenes::script>("res://scripts/camera_movement.lua");
//     camera_movement.set("move_speed", 5.0f);

//     auto& camera_transform = camera.get_component<sbx::math::transform>();
//     camera_transform.set_position(sbx::math::vector3{0.0f, 0.0f, 5.0f});

//     // [Todo] KAJ 2023-08-16 15:30 - This should probably be done automatically
//     scene.start();

//     window.show();
//   }

//   ~demo_application() override {

//   }

//   auto update() -> void  {
//     if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
//       sbx::core::engine::quit();
//     }
//   }

// }; // class demo_application

#include <AL/al.h>
#include <AL/alc.h>

#include <AudioFile.h>

class demo_application : public sbx::core::application {

public:

  demo_application() {
    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

    assets_module.set_asset_directory("./demo/assets");

    ALCdevice* _device;
    ALCcontext* _context;
    
    _device = alcOpenDevice(nullptr);

    if (!_device) {
      throw std::runtime_error("Failed to open audio device");
    }

    _context = alcCreateContext(_device, nullptr);

    if (!alcMakeContextCurrent(_context)) {
      throw std::runtime_error("Failed to make audio context current");
    }

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Failed to initialize audio context");
    }

    auto source = ALuint{0};

    alGenSources(1, &source);

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Failed to generate audio source");
    }

    alSourcef(source, AL_PITCH, 1);

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Failed to set audio source pitch");
    }

    alSourcef(source, AL_GAIN, 1);

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Failed to set audio source gain");
    }

    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Failed to set audio source position");
    }

    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Failed to set audio source velocity");
    }

    alSourcei(source, AL_LOOPING, AL_TRUE);

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Failed to set audio source looping");
    }

    auto buffer = ALuint{0};

    alGenBuffers(1, &buffer);

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Failed to generate audio buffer");
    }

    auto audio_file = AudioFile<float>{};

    auto path = assets_module.asset_path("res://audio/ambient.wav");

    sbx::core::logger::info("Loading audio file: {}", path.string());

    if (!audio_file.load(path.string())) {
      throw std::runtime_error(fmt::format("Failed to load audio file: {}", path.string()));
    }

    audio_file.printSummary();

    alcCloseDevice(_device);
  }

  ~demo_application() override = default;

  auto update() -> void  {

  }

}; // class demo_application

auto sbx::core::create_application() -> std::unique_ptr<sbx::core::application> {
  return std::make_unique<demo_application>();
}
