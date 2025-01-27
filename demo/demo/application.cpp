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

  auto& scene = scenes_module.create_scene("demo/assets/scenes/scene.yaml");

  // Terrain

  auto& terrain_module = sbx::core::engine::get_module<demo::terrain_module>();

  terrain_module.load_terrain_in_scene(scene);

  // Trees

  auto tree_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::color::white, sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.0f, 0.0f}, _texture_ids["birch_leafs_albedo"]});
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{1, sbx::math::color::white, sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.0f, 0.2f}, _texture_ids["birch_stem_albedo"]});
  // tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::color{0.3f, 0.6f, 0.2f, 1.0f}, sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.2f, 0.0f}, _texture_ids["white"]});
  // tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{1, sbx::math::color{0.4f, 0.2f, 0.1f, 1.0f}, sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.2f}, _texture_ids["white"]});

  const auto grid_size = sbx::math::vector2{5.0f, 5.0f};
  const auto cell_size = sbx::math::vector2{10.0f, 10.0f};
  const auto offset = grid_size * cell_size * 0.5f;

  auto forrest = scene.create_node("Forrest");

  for (auto y : std::views::iota(0u, grid_size.y())) {
    for (auto x : std::views::iota(0u, grid_size.x())) {
      auto tree = scene.create_child_node(forrest, fmt::format("Tree{}{}", x, y));

      tree.add_component<sbx::scenes::static_mesh>(_mesh_ids["birch"], tree_submeshes);

      const auto position = (sbx::math::vector2{x, y} * cell_size - offset) + (sbx::math::vector2{sbx::math::random::next<std::float_t>(0.0f, 1.0f), sbx::math::random::next<std::float_t>(0.0f, 1.0f)} * cell_size);

      auto& tree_transform = tree.get_component<sbx::math::transform>();
      tree_transform.set_position(sbx::math::vector3{position.x(), 0.0f, position.y()});
      tree_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});
      tree_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{sbx::math::random::next<std::float_t>(0.0f, 360.0f)});
    }
  }

  // Camera
  auto camera = scene.camera();

  const auto position = sbx::math::vector3{10.0f, 10.0f, 10.0f};

  camera.get_component<sbx::math::transform>().set_position(position);
  camera.get_component<sbx::math::transform>().look_at(sbx::math::vector3::zero);

  window.show();
}

auto application::update() -> void  {
  if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
    sbx::core::engine::quit();
    return;
  }

  _camera_controller.update();

  const auto delta_time = sbx::core::engine::delta_time();

  _rotation += sbx::math::degree{45} * delta_time;
}

auto application::fixed_update() -> void {

}

} // namespace demo
