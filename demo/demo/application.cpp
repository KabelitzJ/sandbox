#include <demo/application.hpp>

#include <nlohmann/json.hpp>

#include <libsbx/math/color.hpp>

#include <demo/renderer.hpp>
#include <demo/line.hpp>

namespace demo {

application::application()
: sbx::core::application{},
  _terrain{sbx::math::vector2f{0.0f, 0.0f}},
  _rotation{sbx::math::degree{0}} {
  // Renderer

  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  graphics_module.set_renderer<renderer>();

  // Textures

  auto texture_map = nlohmann::json::parse(std::ifstream{"demo/assets/textures/texture_map.json"});

  auto textures = texture_map["textures"];

  for (const auto& entry : textures) {
    const auto name = entry["name"].get<std::string>();
    const auto path = entry["path"].get<std::string>();

    const auto id = graphics_module.add_asset<sbx::graphics::image2d>(path);

    _texture_ids.emplace(name, id);
  }

  // Meshes

  auto mesh_map = nlohmann::json::parse(std::ifstream{"demo/assets/meshes/mesh_map.json"});

  auto meshes = mesh_map["meshes"];

  for (const auto& entry : meshes) {
    const auto name = entry["name"].get<std::string>();
    const auto path = entry["path"].get<std::string>();

    const auto id = graphics_module.add_asset<sbx::models::mesh>(path);

    _mesh_ids.emplace(name, id);
  }

  _mesh_ids.emplace("plane", graphics_module.add_asset<sbx::models::mesh>(_generate_plane(sbx::math::vector2u{10u, 10u}, sbx::math::vector2u{10u, 10u})));

  // Window

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  // Scene

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.create_scene();

  _terrain.set_texture(_texture_ids["checkerboard"]);

  // Plane

  auto plane = scene.create_node("Plane");

  auto plane_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  plane_submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::color::white, _texture_ids[texture_map["plane_texture"].get<std::string>()]});

  plane.add_component<sbx::scenes::static_mesh>(_mesh_ids["plane"], plane_submeshes);

  auto& plane_transform = plane.get_component<sbx::math::transform>();
  plane_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});

  plane.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  plane.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-50.0f, 0.0f, -50.0f}, sbx::math::vector3{50.0f, 0.0f, 50.0f}});

  // Dragon

  auto dragon = scene.create_node("Dragon");

  auto dragon_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  // dragon_submeshes.push_back(sbx::scenes::static_mesh::submesh{0, checkerboard_id, sbx::math::color{0.62f, 0.14f, 0.16f, 1.0f}});
  // dragon_submeshes.push_back(sbx::scenes::static_mesh::submesh{1, white_id, sbx::math::color{0.0f, 0.64f, 0.42f, 1.0f}});
  // dragon_submeshes.push_back(sbx::scenes::static_mesh::submesh{0, _texture_ids["checkerboard"], sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f}});
  dragon_submeshes.push_back(sbx::scenes::static_mesh::submesh{1, sbx::math::color{0.62f, 0.14f, 0.16f, 1.0f}, _texture_ids["white"]});
  // dragon_submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::color::white, _texture_ids["checkerboard"]});
  // dragon_submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::color{1.0, 0.0, 0.0, 1.0}, _texture_ids["white"], _texture_ids["brick_wall_normal_map"]});

  dragon.add_component<sbx::scenes::static_mesh>(_mesh_ids["dragon"], dragon_submeshes);

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

struct ball_tag { };

auto application::update() -> void  {
  if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
    sbx::core::engine::quit();
    return;
  }

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.scene();

  auto camera_node = scene.camera();

  auto& camera = camera_node.get_component<sbx::scenes::camera>();

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
}

auto application::fixed_update() -> void {

}

auto application::_generate_plane(const sbx::math::vector2u& tile_count, const sbx::math::vector2u& tile_size) -> std::unique_ptr<sbx::models::mesh> {
  auto vertices = std::vector<sbx::models::vertex3d>{};
  auto indices = std::vector<std::uint32_t>{};

  // Generate vertices

  const auto offset = sbx::math::vector2{static_cast<std::float_t>(tile_count.x() * tile_size.x() / 2.0f), static_cast<std::float_t>(tile_count.y() * tile_size.y() / 2.0f)};

  for (auto y = 0u; y < tile_count.y() + 1u; ++y) {
    for (auto x = 0u; x < tile_count.x() + 1u; ++x) {
      const auto position = sbx::math::vector3{static_cast<std::float_t>(x * tile_size.x() - offset.x()), 0.0f, static_cast<std::float_t>(y * tile_size.y() - offset.y())};
      const auto normal = sbx::math::vector3::up;
      const auto uv = sbx::math::vector2{static_cast<std::float_t>(x % 2), static_cast<std::float_t>(y % 2)};

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
