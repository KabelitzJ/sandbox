#include <demo/demo_application.hpp>

#include <libsbx/math/color.hpp>

#include <demo/demo_renderer.hpp>

namespace demo {

demo_application::demo_application()
: sbx::core::application{},
  _rotation{sbx::math::degree{0}},
  _orbit_angle{sbx::math::degree{90}}, 
  _tilt_angle{sbx::math::degree{30}},
  _target{sbx::math::vector3{0.0f, 0.0f, 0.0f}},
  _zoom{30.0f} {
  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  graphics_module.set_renderer<demo_renderer>();

  auto texture_ids = std::vector<sbx::math::uuid>{};

  const auto prototype_white_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/prototype_white.png");
  const auto base_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/base.png");
  const auto grid_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/grid.png");

  texture_ids.push_back(prototype_white_id);
  texture_ids.push_back(base_id);
  texture_ids.push_back(grid_id);

  const auto monkey_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/suzanne.obj");

  sbx::core::logger::debug("prototype_white_id: {}", prototype_white_id);
  sbx::core::logger::debug("monkey_id: {}", monkey_id);

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.create_scene();

  for (auto i : std::views::iota(0, 5)) {
    auto monkey = scene.create_node(fmt::format("Monkey{}", i));

    auto submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};

    submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::random_element(texture_ids)});

    monkey.add_component<sbx::scenes::static_mesh>(monkey_id, submeshes);

    monkey.get_component<sbx::math::transform>().set_position(sbx::math::vector3{static_cast<std::float_t>(i - 2) * 3.0f, 0.0f, 0.0f});

    sbx::core::logger::debug("Created {} with id: {}", std::string{monkey.get_component<sbx::scenes::tag>()}, sbx::math::uuid{monkey.get_component<sbx::scenes::id>()});
  }

  auto camera = scene.camera();

  camera.get_component<sbx::math::transform>().set_position(sbx::math::vector3{0.0f, 2.0f, 8.0f});
  camera.get_component<sbx::math::transform>().look_at(sbx::math::vector3::zero);
  // camera.get_component<sbx::math::transform>().set_rotation(sbx::math::vector3::right, sbx::math::degree{26.565});

  auto& ui_module = sbx::core::engine::get_module<sbx::ui::ui_module>();

  auto& container = ui_module.container();

  static auto font = sbx::ui::font{"demo/assets/fonts/JetBrainsMono-Medium.ttf", sbx::ui::pixels{16}};

  auto label = container.add_widget<sbx::ui::label>("Hello, World!", sbx::math::vector2u{10, 10}, &font, 1.0f, sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  window.show();
}

auto demo_application::update() -> void  {
  if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
    sbx::core::engine::quit();
    return;
  }

  const auto delta_time = sbx::core::engine::delta_time();

  // _rotation += sbx::math::degree{45} * delta_time;

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.scene();

  for (auto& node : scene.query<sbx::scenes::static_mesh>()) {
    auto& transform = node.get_component<sbx::math::transform>();
    transform.set_rotation(sbx::math::vector3::up, _rotation);
  }

  auto camera = scene.camera();

  auto& transform = camera.get_component<sbx::math::transform>();

  auto movement = sbx::math::vector3{};

  const auto local_forward = sbx::math::vector3::cross(sbx::math::vector3::up, transform.right()).normalize();
  const auto local_right = sbx::math::vector3::cross(sbx::math::vector3::up, transform.forward()).normalize();

  if (sbx::devices::input::is_key_down(sbx::devices::key::w)) {
    movement += local_forward;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::s)) {
    movement -= local_forward;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::a)) {
    movement += local_right;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::d)) {
    movement -= local_right;
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::q)) {
    _orbit_angle += sbx::math::degree{45.0f * delta_time.value()};
  }

  if (sbx::devices::input::is_key_down(sbx::devices::key::e)) {
    _orbit_angle -= sbx::math::degree{45.0f * delta_time.value()};
  }

  _target += movement * 10.0f * delta_time.value();

  const auto tilt_angle_rad = _tilt_angle.to_radians().value();

  const auto radius = std::cos(tilt_angle_rad) * _zoom;
  const auto height = std::sin(tilt_angle_rad) * _zoom;

  const auto orbit_angle_rad = _orbit_angle.to_radians().value();

  const auto x = std::cos(orbit_angle_rad) * radius;
  const auto z = std::sin(orbit_angle_rad) * radius;

  transform.set_position(_target + sbx::math::vector3{x, height, z});
  transform.look_at(_target);
}

} // namespace demo
