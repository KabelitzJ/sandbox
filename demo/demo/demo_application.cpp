#include <demo/demo_application.hpp>

#include <demo/demo_renderer.hpp>

namespace demo {

demo_application::demo_application()
: sbx::core::application{},
  _rotation{sbx::math::degree{0}},
  _font{"demo/assets/fonts/JetBrainsMono-Medium.ttf"} {
  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  graphics_module.set_renderer<demo_renderer>();

  const auto prototype_white_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/prototype_white.png");
  const auto cube_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/suzanne.obj");

  sbx::core::logger::debug("prototype_white_id: {}", prototype_white_id);
  sbx::core::logger::debug("cube_id: {}", cube_id);

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.create_scene();

  auto cube = scene.create_node("Cube");

  cube.add_component<sbx::scenes::static_mesh>(cube_id, std::vector<sbx::scenes::static_mesh::submesh>{{0, prototype_white_id}});

  cube.get_component<sbx::math::transform>().set_rotation(sbx::math::vector3::up, _rotation);

  sbx::core::logger::debug("Created cube with id: {}", sbx::math::uuid{cube.get_component<sbx::scenes::id>()});

  auto camera = scene.camera();

  camera.get_component<sbx::math::transform>().set_position(sbx::math::vector3{0.0f, 4.0f, 8.0f});
  camera.get_component<sbx::math::transform>().set_rotation(sbx::math::vector3::right, sbx::math::degree{26.565});

  window.show();
}

auto demo_application::update() -> void  {
  if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
    sbx::core::engine::quit();
    return;
  }

  const auto dt = sbx::core::engine::delta_time();

  _rotation += sbx::math::degree{45} * dt;

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.scene();

  auto camera = scene.camera();

  for (auto& node : scene.query<sbx::scenes::static_mesh>()) {
    node.get_component<sbx::math::transform>().set_rotation(sbx::math::vector3::up, _rotation);
  }

  auto& camera_transform = camera.get_component<sbx::math::transform>();

  const auto move_speed = 0.5f;

  auto movement = sbx::math::vector3{};

  if (sbx::devices::input::is_key_down(sbx::devices::key::w)) {
    movement.z() -= move_speed * dt;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::s)) {
    movement.z() += move_speed * dt;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::a)) {
    movement.x() -= move_speed * dt;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::d)) {
    movement.x() += move_speed * dt;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::q)) {
    movement.y() += move_speed * dt;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::e)) {
    movement.y() -= move_speed * dt;
  }

  camera_transform.move_by(sbx::math::vector3::normalized(movement));
}

} // namespace demo
