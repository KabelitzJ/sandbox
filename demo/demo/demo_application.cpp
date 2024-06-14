#include <demo/demo_application.hpp>

#include <libsbx/math/color.hpp>

#include <demo/demo_renderer.hpp>

namespace demo {

demo_application::demo_application()
: sbx::core::application{},
  _rotation{sbx::math::degree{0}} {
  // Renderer

  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  graphics_module.set_renderer<demo_renderer>();

  // Textures

  const auto prototype_white_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/prototype_white.png");
  const auto prototype_black_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/prototype_black.png");
  const auto base_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/base.png");
  const auto grid_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/grid.png");
  const auto wood_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/wood.png"); 
  const auto white_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/white.png");

  _texture_ids.push_back(prototype_white_id);
  _texture_ids.push_back(prototype_black_id);  
  _texture_ids.push_back(base_id);
  _texture_ids.push_back(grid_id);
  _texture_ids.push_back(wood_id);
  _texture_ids.push_back(white_id);

  // Meshes

  const auto monkey_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/suzanne.obj");
  const auto plane_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/plane.obj");
  const auto sphere_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/sphere.obj");

  _mesh_ids.push_back(monkey_id);
  _mesh_ids.push_back(plane_id);
  _mesh_ids.push_back(sphere_id);

  // Window

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  // Scene

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.create_scene();

  // Plane

  auto plane = scene.create_node("Plane");

  auto submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};

  submeshes.push_back(sbx::scenes::static_mesh::submesh{0, prototype_white_id});

  plane.add_component<sbx::scenes::static_mesh>(plane_id, submeshes);
  
  auto& plane_transform = plane.get_component<sbx::math::transform>();
  plane_transform.set_position(sbx::math::vector3{0.0f, 0.0f, 0.0f});
  plane_transform.set_scale(sbx::math::vector3{10.0f, 1.0f, 10.0f});

  _plane_id = plane.get_component<sbx::scenes::id>();

  // Sphere

  auto sphere = scene.create_node("Sphere");

  sphere.add_component<sbx::scenes::gizmo>(sphere_id, 0u, sbx::math::color{1.0f, 1.0f, 0.0f, 1.0f});

  auto& sphere_transform = sphere.get_component<sbx::math::transform>();

  sphere_transform.set_scale(sbx::math::vector3{0.2f, 0.2f, 0.2f});

  _sphere_id = sphere.get_component<sbx::scenes::id>();

  // Monkeys

  for (auto i : std::views::iota(0, 5)) {
    auto monkey = scene.create_node(fmt::format("Monkey{}", i));

    auto submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};

    submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::random_element(_texture_ids)});

    monkey.add_component<sbx::scenes::static_mesh>(monkey_id, submeshes);

    auto& transform = monkey.get_component<sbx::math::transform>();
    transform.set_position(sbx::math::vector3{static_cast<std::float_t>(i - 2) * 3.0f, 2.0f, 0.0f});

    _monkey_ids.push_back(monkey.get_component<sbx::scenes::id>());
  }

  // Camera

  auto camera = scene.camera();

  camera.get_component<sbx::math::transform>().set_position(sbx::math::vector3{0.0f, 2.0f, 8.0f});
  camera.get_component<sbx::math::transform>().look_at(sbx::math::vector3::zero);

  // UI

  auto& ui_module = sbx::core::engine::get_module<sbx::ui::ui_module>();

  auto& container = ui_module.container();

  static auto font = sbx::ui::font{"demo/assets/fonts/JetBrainsMono-Medium.ttf", sbx::ui::pixels{16}};

  container.add_widget<sbx::ui::label>("Hello, World!", sbx::math::vector2u{10, 10}, &font, 1.0f, sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  _delta_time_label = container.add_widget<sbx::ui::label>("Delta: 0", sbx::math::vector2u{10, 40}, &font, 1.0f, sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});
  _fps_label = container.add_widget<sbx::ui::label>("FPS: 0", sbx::math::vector2u{10, 70}, &font, 1.0f, sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  window.show();
}

auto demo_application::update() -> void  {
  if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
    sbx::core::engine::quit();
    return;
  }

  const auto delta_time = sbx::core::engine::delta_time();

  _delta_time_label->set_text(fmt::format("Delta: {:.4f}ms", sbx::units::quantity_cast<sbx::units::millisecond>(delta_time).value()));

  _time += delta_time;
  _frames++;

  if (_time >= sbx::units::second{1}) {
    _fps_label->set_text(fmt::format("FPS: {}", _frames));
    _time = sbx::units::second{0};
    _frames = 0;
  }

  _rotation += sbx::math::degree{45} * delta_time;

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.scene();

  for (const auto& monkey_id : _monkey_ids) {
    auto monkey = scene.find_node(monkey_id);

    auto& transform = monkey->get_component<sbx::math::transform>();

    transform.set_rotation(sbx::math::vector3::up, _rotation);
  }

  _camera_controller.update();

  auto sphere = scene.find_node(_sphere_id);

  auto& sphere_transform = sphere->get_component<sbx::math::transform>();

  sphere_transform.set_position(_camera_controller.target());
}

} // namespace demo
