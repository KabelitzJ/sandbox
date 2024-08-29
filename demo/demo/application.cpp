#include <demo/application.hpp>

#include <nlohmann/json.hpp>

#include <libsbx/math/color.hpp>
#include <libsbx/math/noise.hpp>

#include <demo/renderer.hpp>
#include <demo/line.hpp>

#include <demo/terrain/terrain_subrenderer.hpp>

namespace demo {

application::application()
: sbx::core::application{},
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

  // _mesh_ids.emplace("plane", graphics_module.add_asset<sbx::models::mesh>(_generate_plane(sbx::math::vector2u{10u, 10u}, sbx::math::vector2u{10u, 10u})));
  _mesh_ids.emplace("terrain", graphics_module.add_asset<demo::mesh>(_generate_terrain(sbx::math::vector2u{1000u, 1000u}, sbx::math::vector2u{10u, 10u})));

  // Window

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  // Scene

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.create_scene();

  // Terrain

  auto terrain = scene.create_node("Terrain");

  terrain.add_component<demo::terrain>(_mesh_ids["terrain"], sbx::math::color::white, _texture_ids["grass_albedo"], _texture_ids["grass_normal"], _texture_ids["dirt_albedo"], _texture_ids["dirt_normal"]);

  auto& terrain_transform = terrain.get_component<sbx::math::transform>();
  // terrain_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});

  // terrain.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  // terrain.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-50.0f, 0.0f, -50.0f}, sbx::math::vector3{50.0f, 0.0f, 50.0f}});

  // Dragon

  auto dragon = scene.create_node("Dragon");

  auto dragon_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  dragon_submeshes.push_back(sbx::scenes::static_mesh::submesh{1, sbx::math::color{0.62f, 0.14f, 0.16f, 1.0f}, _texture_ids["white"]});

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

auto application::_generate_terrain(const sbx::math::vector2u& size, const sbx::math::vector2u& tile_size) -> std::unique_ptr<demo::mesh> {
  auto vertices = std::vector<demo::mesh::vertex_type>{};
  auto indices = std::vector<demo::mesh::index_type>{};
  auto heights = std::vector<std::float_t>{};

  // Generate vertices

  const auto tile_count = sbx::math::vector2u{size.x() / tile_size.x(), size.y() / tile_size.y()};
  const auto vertex_count = sbx::math::vector2u{tile_count.x() + 1u, tile_count.y() + 1u};

  const auto offset = sbx::math::vector2{static_cast<std::float_t>(size.x() / 2.0f), static_cast<std::float_t>(size.y() / 2.0f)};

  for (auto y = 0u; y < vertex_count.y(); ++y) {
    for (auto x = 0u; x < vertex_count.x(); ++x) {
      const auto position = sbx::math::vector2{static_cast<std::float_t>(x * tile_size.x() - offset.x()), static_cast<std::float_t>(y * tile_size.y() - offset.y())};
      const auto index = y * (vertex_count.x()) + x;
      const auto normal = sbx::math::vector3::up;
      const auto uv = sbx::math::vector2{static_cast<std::float_t>(x), static_cast<std::float_t>(y)};

      vertices.emplace_back(position, index, normal, uv);

      const auto height = sbx::math::noise::fractal(static_cast<std::float_t>(x) / (4.0f * tile_size.x()), static_cast<std::float_t>(y) / (4.0f * tile_size.y()), 4u) * 10.0f;

      heights.push_back(height);
    }
  }

  // Calculate indices

  const auto count = tile_count.x() + 1u;

  for (auto i = 0u; i < count * count - count; ++i) {
    if ((i + 1u) % count == 0) {
      continue;
    }

    indices.emplace_back(i);
    indices.emplace_back(i + count);
    indices.emplace_back(i + count + 1u);

    indices.emplace_back(i);
    indices.emplace_back(i + count + 1u);
    indices.emplace_back(i + 1u);
  }

  // Calculate normals

  for (auto i = 0u; i < indices.size(); i += 3) {
    const auto index0 = indices[i];
    const auto index1 = indices[i + 1];
    const auto index2 = indices[i + 2];

    auto& vertex0 = vertices[index0];
    auto& vertex1 = vertices[index1];
    auto& vertex2 = vertices[index2];

    const auto edge1 = vertex1.position - vertex0.position;
    const auto edge2 = vertex2.position - vertex0.position;

    const auto normal = sbx::math::vector3::normalized(sbx::math::vector3::cross(edge1, edge2));

    vertex0.normal += normal;
    vertex1.normal += normal;
    vertex2.normal += normal;
  }

  return std::make_unique<demo::mesh>(std::move(vertices), std::move(indices), std::move(heights), vertex_count);
}

} // namespace demo
