#include <demo/demo_application.hpp>

// #include <demo/demo_renderer.hpp>

namespace demo {

demo_application::demo_application()
: sbx::core::application{} {
  // auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();
  
  // const auto& cli = sbx::core::engine::cli();

  // assets_module.set_asset_directory("./demo/assets");

  // sbx::core::logger::info("Asset directory: {}", assets_module.asset_directory().string());

  // auto base_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/base.png");
  // auto default_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/default.png");
  // auto grid_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/grid.png");
  // auto prototype_black_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/prototype_black.png");
  // auto white_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/white.png");

  // _texture_id = base_id;

  // auto monkey_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/suzanne.obj");
  // auto sphere_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/sphere.obj");
  // auto cube_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/cube.obj");
  // auto tree_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/tree.obj");
  // auto tree_1_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/tree_1.obj");
  // auto plane_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/plane.obj");

  // _mesh_id = sphere_id;

  // auto font_jet_brains_mono_id = assets_module.load_asset<sbx::ui::font>("res://fonts/JetBrainsMono-Medium.ttf", sbx::ui::pixels{18u});
  // auto font_roboto_id = assets_module.load_asset<sbx::ui::font>("res://fonts/Roboto-Regular.ttf", sbx::ui::pixels{18u});

  // auto ambience_birds_sound_id = assets_module.load_asset<sbx::audio::sound_buffer>("res://audio/ambience.wav");
  // auto forest_sound_id = assets_module.load_asset<sbx::audio::sound_buffer>("res://audio/forest.wav");

  // auto& ui_module = sbx::core::engine::get_module<sbx::ui::ui_module>();

  // auto& container = ui_module.container();

  // _label_fps = container.add_widget<sbx::ui::label>("FPS:  0", sbx::math::vector2u{25, 25}, font_jet_brains_mono_id, 0.75f, sbx::math::color{0.53f, 0.01f, 0.01f, 1.0f});
  // _label_delta_time = container.add_widget<sbx::ui::label>("Delta time: 0 ms", sbx::math::vector2u{25, 50}, font_jet_brains_mono_id, 0.75f, sbx::math::color{0.53f, 0.01f, 0.01f, 1.0f});

  // auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  // graphics_module.set_renderer<demo_renderer>();

  // auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  // auto& window = devices_module.window();

  // window.set_icon("res://icons/sandbox.png");

  // window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
  //   sbx::core::engine::quit();
  // };

  // auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  // auto& scene = scenes_module.load_scene("res://scenes/demo.yaml");

  // auto floor_id = assets_module.add_asset<sbx::models::mesh>(_generate_plane(sbx::math::vector2u{50u, 50u}, sbx::math::vector2u{1u, 1u}));

  // auto white_id = assets_module.try_get_asset_id("res://textures/white.png");

  // auto floor_node = scene.create_node("Floor", sbx::math::transform{sbx::math::vector3::zero, sbx::math::vector3::zero, sbx::math::vector3::one});
  // floor_node.add_component<sbx::scenes::static_mesh>(floor_id, std::vector<sbx::scenes::static_mesh::submesh>{{0, *white_id, sbx::math::color{0.37f, 0.43f, 0.32f, 1.0f}}});

  // // [Todo] KAJ 2023-08-16 15:30 - This should probably be done automatically
  // scene.start();

  // window.show();
}

auto demo_application::update() -> void  {
  // if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
  //   sbx::core::engine::quit();
  // }

  // auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  // auto& scene = scenes_module.scene();

  // if (_flag && !_cube) {
  //   _cube = scene.create_node("Cube", sbx::math::transform{sbx::math::vector3{-5.0f, 10.0f, 0.0f}, sbx::math::vector3::zero, sbx::math::vector3::one});

  //   _cube->add_component<sbx::scenes::static_mesh>(_mesh_id, _texture_id);

  //   // auto& script = _cube->add_component<sbx::scenes::script>("res://scripts/rotate.lua");
  //   // script.set("speed", -120.0f);

  //   auto& rigidbody = _cube->add_component<sbx::physics::rigidbody>(1.0f, 0.75f, false);
  //   rigidbody.set_acceleration(sbx::math::vector3{0.0f, -9.81f, 0.0f});

  //   _cube->add_component<sbx::physics::box_collider>(sbx::math::vector3{1.0f, 1.0f, 1.0f});
  // } else if (!_flag && _cube) {
  //   scene.destroy_node(*_cube);
  //   _cube.reset();
  // }

  // auto camera_node = scene.camera();
  // auto& camera = camera_node.get_component<sbx::scenes::camera>();

  // if (sbx::devices::input::is_key_pressed(sbx::devices::key::r)) {
  //   camera.set_field_of_view(sbx::math::degree{75.0f});
  // } else {
  //   const auto& scroll = sbx::devices::input::scroll_delta();

  //   auto field_of_view = camera.field_of_view().to_degrees() - scroll.y * 10.0f;

  //   if (field_of_view > 75.0f) {
  //     field_of_view = sbx::math::degree{75.0f};
  //   } else if (field_of_view < 30.0f) {
  //     field_of_view = sbx::math::degree{30.0f};
  //   }

  //   camera.set_field_of_view(field_of_view);
  // }

  // const auto delta_time = sbx::core::engine::delta_time();

  // _time += sbx::units::second{delta_time};

  // if (_time >= sbx::units::second{1.0f}) {
  //   _label_fps->set_text(fmt::format("FPS: {}", _frames));
  //   _time -= sbx::units::second{1.0f};
  //   _frames = 0;
  // } else {
  //   ++_frames;
  // }

  // _label_delta_time->set_text(fmt::format("Delta time: {:.2f} ms", sbx::units::quantity_cast<sbx::units::millisecond>(delta_time).value()));
}

// auto demo_application::_generate_plane(const sbx::math::vector2u& tile_count, const sbx::math::vector2u& tile_size) -> std::unique_ptr<sbx::models::mesh> {
//   auto vertices = std::vector<sbx::models::vertex3d>{};
//   auto indices = std::vector<std::uint32_t>{};

//   // Generate vertices

//   const auto offset = sbx::math::vector2{static_cast<std::float_t>(tile_count.x * tile_size.x / 2.0f), static_cast<std::float_t>(tile_count.y * tile_size.y / 2.0f)};

//   for (auto y = 0u; y < tile_count.y + 1u; ++y) {
//     for (auto x = 0u; x < tile_count.x + 1u; ++x) {
//       const auto position = sbx::math::vector3{static_cast<std::float_t>(x * tile_size.x - offset.x), 0.0f, static_cast<std::float_t>(y * tile_size.y - offset.y)};
//       const auto normal = sbx::math::vector3::up;
//       const auto uv = sbx::math::vector2{static_cast<std::float_t>(x), static_cast<std::float_t>(y)};

//       vertices.emplace_back(position, normal, uv);
//     }
//   }

//   // Calculate indices

//   const auto vertex_count = tile_count.x + 1u;

//   for (auto i = 0u; i < vertex_count * vertex_count - vertex_count; ++i) {
//     if ((i + 1u) % vertex_count == 0) {
//       continue;
//     }

//     indices.emplace_back(i);
//     indices.emplace_back(i + vertex_count);
//     indices.emplace_back(i + vertex_count + 1u);

//     indices.emplace_back(i);
//     indices.emplace_back(i + vertex_count + 1u);
//     indices.emplace_back(i + 1u);
//   }

//   return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices));
// }

} // namespace demo
