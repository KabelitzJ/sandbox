#include <demo/application.hpp>

#include <nlohmann/json.hpp>

#include <libsbx/math/color.hpp>
#include <libsbx/math/noise.hpp>

#include <demo/renderer.hpp>
#include <demo/line.hpp>

#include <demo/terrain/terrain_subrenderer.hpp>
#include <demo/terrain/terrain_module.hpp>
#include <demo/terrain/chunk.hpp>

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

  auto& terrain_module = sbx::core::engine::get_module<demo::terrain_module>();

  terrain_module.load_terrain_in_scene(scene);

  // Monkey

  auto monkey = scene.create_node("Monkey");

  auto monkey_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  monkey_submeshes.push_back(sbx::scenes::static_mesh::submesh{1, sbx::math::color{0.62f, 0.14f, 0.16f, 1.0f}, sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.0f, 0.0f}, _texture_ids["white"]});

  monkey.add_component<sbx::scenes::static_mesh>(_mesh_ids["dragon"], monkey_submeshes);

  auto& monkey_transform = monkey.get_component<sbx::math::transform>();
  monkey_transform.set_position(sbx::math::vector3{0.0f, 5.0f, 0.0f});
  monkey_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

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

  // auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  // auto& scene = scenes_module.scene();

  // auto camera_node = scene.camera();

  // auto& camera = camera_node.get_component<sbx::scenes::camera>();

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

// static auto _find_midpoint(const std::uint32_t i1, const std::uint32_t i2, std::vector<sbx::models::vertex3d>& vertices, std::unordered_map<std::uint64_t, std::uint32_t>& cache) -> std::uint32_t {
//   const auto is_first_smaller = i1 < i2;
//   const auto smaller_index = is_first_smaller ? i1 : i2;
//   const auto greater_index = is_first_smaller ? i2 : i1;
//   const auto key = (static_cast<std::uint64_t>(smaller_index) << 32) + greater_index;

//   if (const auto it = cache.find(key); it != cache.end()) {
//     return it->second;
//   }

//   const auto& position1 = vertices[i1].position;
//   const auto& position2 = vertices[i2].position;

//   const auto midpoint = sbx::math::vector3::normalized((position1 + position2) / 2.0f);

//   auto vertex = sbx::models::vertex3d{};
//   vertex.position = midpoint;
//   vertex.normal = midpoint;
//   vertex.uv = sbx::math::vector2{atan2(midpoint.z(), midpoint.x()) / (2.0f * std::numbers::pi_v<std::float_t>) + 0.5f, asin(midpoint.y()) / std::numbers::pi_v<std::float_t> + 0.5f};

//   const auto index = static_cast<std::uint32_t>(vertices.size());
//   vertices.push_back(vertex);

//   cache.emplace(key, index);

//   return index;
// }

// auto application::_generate_icosphere(const std::float_t radius, const std::uint32_t subdivisions) -> std::unique_ptr<sbx::models::mesh> {
//   auto vertices = std::vector<sbx::models::vertex3d>{};
//   auto indices = std::vector<std::uint32_t>{};

//   // Generate icosphere

//   const auto t = (1.0f + std::sqrt(5.0f)) / 2.0f;

//   auto base_positions = std::vector<sbx::math::vector3>{
//     sbx::math::vector3{-1.0f,  t,  0.0f},
//     sbx::math::vector3{ 1.0f,  t,  0.0f},
//     sbx::math::vector3{-1.0f, -t,  0.0f},
//     sbx::math::vector3{ 1.0f, -t,  0.0f},
//     sbx::math::vector3{ 0.0f, -1.0f,  t},
//     sbx::math::vector3{ 0.0f,  1.0f,  t},
//     sbx::math::vector3{ 0.0f, -1.0f, -t},
//     sbx::math::vector3{ 0.0f,  1.0f, -t},
//     sbx::math::vector3{ t,  0.0f, -1.0f},
//     sbx::math::vector3{ t,  0.0f,  1.0f},
//     sbx::math::vector3{-t,  0.0f, -1.0f},
//     sbx::math::vector3{-t,  0.0f,  1.0f}
//   };

//   for (const auto& position : base_positions) {
//     auto vertex = sbx::models::vertex3d{};

//     vertex.position = sbx::math::vector3::normalized(position);
//     vertex.normal = sbx::math::vector3::normalized(position);
//     vertex.uv = sbx::math::vector2{atan2(position.z(), position.x()) / (2.0f * std::numbers::pi_v<std::float_t>) + 0.5f, asin(position.y()) / std::numbers::pi_v<std::float_t> + 0.5f};

//     vertices.push_back(vertex);
//   }

//   auto base_indices = std::vector<sbx::math::vector3i>{
//     {0, 11,  5}, {0,  5,  1}, {0,  1,  7}, {0,  7, 10}, {0, 10, 11},
//     {1,  5,  9}, {5, 11,  4}, {11, 10,  2}, {10,  7,  6}, {7,  1,  8},
//     {3,  9,  4}, {3,  4,  2}, {3,  2,  6}, {3,  6,  8}, {3,  8,  9},
//     {4,  9,  5}, {2,  4, 11}, {6,  2, 10}, {8,  6,  7}, {9,  8,  1},
//   };

//   for (const auto& face : base_indices) {
//     indices.push_back(face.x());
//     indices.push_back(face.y());
//     indices.push_back(face.z());
//   }

//   auto middle_point_cache = std::unordered_map<std::uint64_t, std::uint32_t>{};

//   for (auto i = 0u; i < subdivisions; ++i) {
//     auto new_indices = std::vector<std::uint32_t>{};

//     for (auto j = 0u; j < indices.size(); j += 3) {
//       const auto i1 = indices[j];
//       const auto i2 = indices[j + 1];
//       const auto i3 = indices[j + 2];

//       const auto a = _find_midpoint(i1, i2, vertices, middle_point_cache);
//       const auto b = _find_midpoint(i2, i3, vertices, middle_point_cache);
//       const auto c = _find_midpoint(i3, i1, vertices, middle_point_cache);

//       new_indices.push_back(i1);
//       new_indices.push_back(a);
//       new_indices.push_back(c);

//       new_indices.push_back(i2);
//       new_indices.push_back(b);
//       new_indices.push_back(a);

//       new_indices.push_back(i3);
//       new_indices.push_back(c);
//       new_indices.push_back(b);

//       new_indices.push_back(a);
//       new_indices.push_back(b);
//       new_indices.push_back(c);
//     }

//     indices = std::move(new_indices);
//   }

//   // const auto strength = 0.7f;
//   // const auto roughness = 1.2f;
//   // const auto min_height = 1.0f;
//   // const auto center = sbx::math::vector3::zero;

//   for (auto& vertex : vertices) {
//     // const auto noise = sbx::math::noise::fractal(vertex.position * roughness + center, 4.0);
//     // const auto height = (noise + 1.0f) * 0.5f * strength;

//     vertex.position += vertex.normal * radius;
//   }

//   return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices));
// }

} // namespace demo
