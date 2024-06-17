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
  const auto plane_id = graphics_module.add_asset<sbx::models::mesh>(_generate_plane(sbx::math::vector2u{1u, 1u}, sbx::math::vector2u{10u, 10u}));
  const auto sphere_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/sphere.obj");
  const auto crate_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/crate.obj");
  const auto tree_2_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/tree_2.obj");

  _mesh_ids.push_back(monkey_id);
  _mesh_ids.push_back(plane_id);
  _mesh_ids.push_back(sphere_id);
  _mesh_ids.push_back(crate_id);
  _mesh_ids.push_back(tree_2_id);

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

  plane.add_component<sbx::scenes::static_mesh>(plane_id, prototype_black_id);
  
  auto& plane_transform = plane.get_component<sbx::math::transform>();
  plane_transform.set_scale(sbx::math::vector3{10.0f, 1.0f, 10.0f});

  // Sphere

  auto sphere = scene.create_node("Sphere");

  sphere.add_component<sbx::scenes::static_mesh>(sphere_id, prototype_black_id);
  
  auto& sphere_transform = sphere.get_component<sbx::math::transform>();
  sphere_transform.set_position(sbx::math::vector3{5.0f, 1.0f, 5.0f});

  // Crate

  auto crate = scene.create_node("Crate");

  crate.add_component<sbx::scenes::static_mesh>(crate_id, wood_id);
  
  auto& crate_transform = crate.get_component<sbx::math::transform>();
  crate_transform.set_position(sbx::math::vector3{-4.0f, 0.0f, 3.5f});
  crate_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});
  crate_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{20});

  // Tree

  auto tree = scene.create_node("Tree");

  auto submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  submeshes.push_back(sbx::scenes::static_mesh::submesh{0, white_id, sbx::math::color{0.38, 0.54, 0.24, 1.0}});
  submeshes.push_back(sbx::scenes::static_mesh::submesh{1, white_id, sbx::math::color{0.47, 0.37, 0.24, 1.0}});

  tree.add_component<sbx::scenes::static_mesh>(tree_2_id, submeshes);
  
  auto& tree_transform = tree.get_component<sbx::math::transform>();
  tree_transform.set_position(sbx::math::vector3{0.0f, 0.0f, -4.0f});
  tree_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // Monkeys

  for (auto i : std::views::iota(0, 5)) {
    auto monkey = scene.create_node(fmt::format("Monkey{}", i));

    auto submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};

    submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::random_element(_texture_ids)});

    monkey.add_component<sbx::scenes::static_mesh>(monkey_id, submeshes);

    auto& monkey_transform = monkey.get_component<sbx::math::transform>();
    monkey_transform.set_position(sbx::math::vector3{static_cast<std::float_t>(i - 2) * 3.0f, 2.0f, 0.0f});

    auto gizmo = scene.create_child_node(monkey, fmt::format("Gizmo{}", i));

    gizmo.add_component<sbx::scenes::gizmo>(sphere_id, 0u, sbx::math::color{sbx::math::random::next<std::float_t>(0.0f, 1.0f), sbx::math::random::next<std::float_t>(0.0f, 1.0f), sbx::math::random::next<std::float_t>(0.0f, 1.0f), 0.5f});

    auto& gizmo_transform = gizmo.get_component<sbx::math::transform>();
    gizmo_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

    _monkey_ids.push_back(monkey.get_component<sbx::scenes::id>());
  }

  // Camera

  auto camera = scene.camera();

  const auto position = sbx::math::vector3{10.0f, 10.0f, 10.0f};

  camera.get_component<sbx::math::transform>().set_position(position);
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

  _camera_controller.update();

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
}

auto demo_application::_generate_plane(const sbx::math::vector2u& tile_count, const sbx::math::vector2u& tile_size) -> std::unique_ptr<sbx::models::mesh> {
  auto vertices = std::vector<sbx::models::vertex3d>{};
  auto indices = std::vector<std::uint32_t>{};

  // Generate vertices

  const auto offset = sbx::math::vector2{static_cast<std::float_t>(tile_count.x() * tile_size.x() / 2.0f), static_cast<std::float_t>(tile_count.y() * tile_size.y() / 2.0f)};

  for (auto y = 0u; y < tile_count.y() + 1u; ++y) {
    for (auto x = 0u; x < tile_count.x() + 1u; ++x) {
      const auto position = sbx::math::vector3{static_cast<std::float_t>(x * tile_size.x() - offset.x()), 0.0f, static_cast<std::float_t>(y * tile_size.y() - offset.y())};
      const auto normal = sbx::math::vector3::up;
      const auto uv = sbx::math::vector2{static_cast<std::float_t>(x), static_cast<std::float_t>(y)};

      vertices.emplace_back(position, normal, uv);
    }
  }

  // Calculate indices

  const auto vertex_count = tile_count.x() + 1u;

  for (auto i = 0u; i < vertex_count * vertex_count - vertex_count; ++i) {
    if ((i + 1u) % vertex_count == 0) {
      continue;
    }

    indices.emplace_back(i);
    indices.emplace_back(i + vertex_count);
    indices.emplace_back(i + vertex_count + 1u);

    indices.emplace_back(i);
    indices.emplace_back(i + vertex_count + 1u);
    indices.emplace_back(i + 1u);
  }

  return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices));
}

} // namespace demo
