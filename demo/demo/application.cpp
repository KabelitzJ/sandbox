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
  _mesh_ids.emplace("terrain", graphics_module.add_asset<demo::mesh>(_generate_terrain(sbx::math::vector2u{200u, 200u}, sbx::math::vector2u{10u, 10u})));
  // _mesh_ids.emplace("icosphere", graphics_module.add_asset<sbx::models::mesh>(_generate_icosphere(20.0f, 4u)));

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

  terrain.add_component<demo::terrain>(_mesh_ids["terrain"], sbx::math::color::white, _texture_ids["grass_albedo"], _texture_ids["dirt_albedo"]);

  auto& terrain_transform = terrain.get_component<sbx::math::transform>();
  terrain_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});

  // terrain.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  // terrain.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-50.0f, 0.0f, -50.0f}, sbx::math::vector3{50.0f, 0.0f, 50.0f}});

  // Monkey

  auto monkey = scene.create_node("Monkey");

  auto monkey_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  monkey_submeshes.push_back(sbx::scenes::static_mesh::submesh{1, sbx::math::color{0.62f, 0.14f, 0.16f, 1.0f}, _texture_ids["white"]});

  monkey.add_component<sbx::scenes::static_mesh>(_mesh_ids["dragon"], monkey_submeshes);

  auto& monkey_transform = monkey.get_component<sbx::math::transform>();
  monkey_transform.set_position(sbx::math::vector3{0.0f, 10.0f, 0.0f});
  monkey_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{-45.0f});
  // dragon_transform.set_scale(sbx::math::vector3{20.0f, 20.0f, 20.0f});

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

  for (auto i = 0u; i < indices.size(); i += 3u) {
    const auto& index0 = indices[i];
    const auto& index1 = indices[i + 1u];
    const auto& index2 = indices[i + 2u];

    const auto& height0 = heights[index0];
    const auto& height1 = heights[index1];
    const auto& height2 = heights[index2];

    auto& vertex0 = vertices[index0];
    auto& vertex1 = vertices[index1];
    auto& vertex2 = vertices[index2];

    const auto position0 = sbx::math::vector3{vertex0.position.x(), height0, vertex0.position.y()};
    const auto position1 = sbx::math::vector3{vertex1.position.x(), height1, vertex1.position.y()};
    const auto position2 = sbx::math::vector3{vertex2.position.x(), height2, vertex2.position.y()};

    const auto edge1 = position1 - position0;
    const auto edge2 = position2 - position0;

    const auto normal = sbx::math::vector3::normalized(sbx::math::vector3::cross(edge1, edge2));

    vertex0.normal += normal;
    vertex1.normal += normal;
    vertex2.normal += normal;
  }

  return std::make_unique<demo::mesh>(std::move(vertices), std::move(indices), std::move(heights), vertex_count);
}

static auto _find_midpoint(const std::uint32_t i1, const std::uint32_t i2, std::vector<sbx::models::vertex3d>& vertices, std::unordered_map<std::uint64_t, std::uint32_t>& cache) -> std::uint32_t {
  const auto is_first_smaller = i1 < i2;
  const auto smaller_index = is_first_smaller ? i1 : i2;
  const auto greater_index = is_first_smaller ? i2 : i1;
  const auto key = (static_cast<std::uint64_t>(smaller_index) << 32) + greater_index;

  if (const auto it = cache.find(key); it != cache.end()) {
    return it->second;
  }

  const auto& position1 = vertices[i1].position;
  const auto& position2 = vertices[i2].position;

  const auto midpoint = sbx::math::vector3::normalized((position1 + position2) / 2.0f);

  auto vertex = sbx::models::vertex3d{};
  vertex.position = midpoint;
  vertex.normal = midpoint;
  vertex.uv = sbx::math::vector2{atan2(midpoint.z(), midpoint.x()) / (2.0f * std::numbers::pi_v<std::float_t>) + 0.5f, asin(midpoint.y()) / std::numbers::pi_v<std::float_t> + 0.5f};

  const auto index = static_cast<std::uint32_t>(vertices.size());
  vertices.push_back(vertex);

  cache.emplace(key, index);

  return index;
}

auto application::_generate_icosphere(const std::float_t radius, const std::uint32_t subdivisions) -> std::unique_ptr<sbx::models::mesh> {
  auto vertices = std::vector<sbx::models::vertex3d>{};
  auto indices = std::vector<std::uint32_t>{};

  // Generate icosphere

  const auto t = (1.0f + std::sqrt(5.0f)) / 2.0f;

  auto base_positions = std::vector<sbx::math::vector3>{
    sbx::math::vector3{-1.0f,  t,  0.0f},
    sbx::math::vector3{ 1.0f,  t,  0.0f},
    sbx::math::vector3{-1.0f, -t,  0.0f},
    sbx::math::vector3{ 1.0f, -t,  0.0f},
    sbx::math::vector3{ 0.0f, -1.0f,  t},
    sbx::math::vector3{ 0.0f,  1.0f,  t},
    sbx::math::vector3{ 0.0f, -1.0f, -t},
    sbx::math::vector3{ 0.0f,  1.0f, -t},
    sbx::math::vector3{ t,  0.0f, -1.0f},
    sbx::math::vector3{ t,  0.0f,  1.0f},
    sbx::math::vector3{-t,  0.0f, -1.0f},
    sbx::math::vector3{-t,  0.0f,  1.0f}
  };

  for (const auto& position : base_positions) {
    auto vertex = sbx::models::vertex3d{};

    vertex.position = sbx::math::vector3::normalized(position);
    vertex.normal = sbx::math::vector3::normalized(position);
    vertex.uv = sbx::math::vector2{atan2(position.z(), position.x()) / (2.0f * std::numbers::pi_v<std::float_t>) + 0.5f, asin(position.y()) / std::numbers::pi_v<std::float_t> + 0.5f};

    vertices.push_back(vertex);
  }

  auto base_indices = std::vector<sbx::math::vector3i>{
    {0, 11,  5}, {0,  5,  1}, {0,  1,  7}, {0,  7, 10}, {0, 10, 11},
    {1,  5,  9}, {5, 11,  4}, {11, 10,  2}, {10,  7,  6}, {7,  1,  8},
    {3,  9,  4}, {3,  4,  2}, {3,  2,  6}, {3,  6,  8}, {3,  8,  9},
    {4,  9,  5}, {2,  4, 11}, {6,  2, 10}, {8,  6,  7}, {9,  8,  1},
  };

  for (const auto& face : base_indices) {
    indices.push_back(face.x());
    indices.push_back(face.y());
    indices.push_back(face.z());
  }

  auto middle_point_cache = std::unordered_map<std::uint64_t, std::uint32_t>{};

  for (auto i = 0u; i < subdivisions; ++i) {
    auto new_indices = std::vector<std::uint32_t>{};

    for (auto j = 0u; j < indices.size(); j += 3) {
      const auto i1 = indices[j];
      const auto i2 = indices[j + 1];
      const auto i3 = indices[j + 2];

      const auto a = _find_midpoint(i1, i2, vertices, middle_point_cache);
      const auto b = _find_midpoint(i2, i3, vertices, middle_point_cache);
      const auto c = _find_midpoint(i3, i1, vertices, middle_point_cache);

      new_indices.push_back(i1);
      new_indices.push_back(a);
      new_indices.push_back(c);

      new_indices.push_back(i2);
      new_indices.push_back(b);
      new_indices.push_back(a);

      new_indices.push_back(i3);
      new_indices.push_back(c);
      new_indices.push_back(b);

      new_indices.push_back(a);
      new_indices.push_back(b);
      new_indices.push_back(c);
    }

    indices = std::move(new_indices);
  }

  // const auto strength = 0.7f;
  // const auto roughness = 1.2f;
  // const auto min_height = 1.0f;
  // const auto center = sbx::math::vector3::zero;

  for (auto& vertex : vertices) {
    // const auto noise = sbx::math::noise::fractal(vertex.position * roughness + center, 4.0);
    // const auto height = (noise + 1.0f) * 0.5f * strength;

    vertex.position += vertex.normal * radius;
  }

  return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices));
}

} // namespace demo
