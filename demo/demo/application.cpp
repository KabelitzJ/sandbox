#include <demo/application.hpp>

#include <nlohmann/json.hpp>

#include <easy/profiler.h>

#include <libsbx/math/color.hpp>
#include <libsbx/math/noise.hpp>

#include <libsbx/assets/assets_module.hpp>

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

  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  // Textures

  auto texture_map = nlohmann::json::parse(std::ifstream{"demo/assets/textures/texture_map.json"});

  auto textures = texture_map["textures"];

  for (const auto& entry : textures) {
    const auto name = entry["name"].get<std::string>();
    const auto path = entry["path"].get<std::string>();

    const auto id = graphics_module.add_asset<sbx::graphics::image2d>(path);

    _texture_ids.emplace(name, id);
  }

  _texture_ids.emplace("tree_1_leaves1", graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/tree_1/leaves1.png"));
  _texture_ids.emplace("tree_1_leaves2", graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/tree_1/leaves2.png"));
  _texture_ids.emplace("tree_1_bark", graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/tree_1/bark.jpg"));

  _texture_ids.emplace("skybox", graphics_module.add_asset<sbx::graphics::cube_image>("demo/assets/skyboxes/clouds"));

  // Meshes

  auto mesh_map = nlohmann::json::parse(std::ifstream{"demo/assets/meshes/mesh_map.json"});

  auto meshes = mesh_map["meshes"];

  for (const auto& entry : meshes) {
    const auto name = entry["name"].get<std::string>();
    const auto path = entry["path"].get<std::string>();

    const auto id = graphics_module.add_asset<sbx::models::mesh>(path);

    _mesh_ids.emplace(name, id);
  }

  _mesh_ids.emplace("tree_1_1", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/tree_1_1/tree_1_1.gltf"));
  _mesh_ids.emplace("tree_1_2", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/tree_1_2/tree_1_2.gltf"));
  _mesh_ids.emplace("tree_1_3", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/tree_1_3/tree_1_3.gltf"));
  _mesh_ids.emplace("tree_1_4", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/tree_1_4/tree_1_4.gltf"));
  _mesh_ids.emplace("bush_5", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/bush_5/bush_5.gltf"));

  // _mesh_ids.emplace("icosphere", graphics_module.add_asset<sbx::models::mesh>(_generate_icosphere(20.0f, 4u)));

  // Window

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  // Scene

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.load_scene("demo/assets/scenes/scene.yaml");

  // Terrain

  auto& terrain_module = sbx::core::engine::get_module<demo::terrain_module>();

  terrain_module.load_terrain_in_scene(scene);

  // Trees

  auto tree_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{0u, false, sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.2f, 0.8f}, _texture_ids["tree_1_bark"]});
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{1u, false, sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.2f, 0.0f}, _texture_ids["tree_1_leaves1"]});
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{2u, false, sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.2f, 0.0f}, _texture_ids["tree_1_leaves2"]});

  const auto grid_size = sbx::math::vector2{25.0f, 25.0f};
  const auto cell_size = sbx::math::vector2{8.0f, 8.0f};
  const auto offset = grid_size * cell_size * 0.5f;

  auto forrest = scene.create_node("Forrest");

  for (auto y : std::views::iota(0u, grid_size.y())) {
    for (auto x : std::views::iota(0u, grid_size.x())) {
      auto tree = scene.create_child_node(forrest, fmt::format("Tree{}{}", x, y));

      scene.add_component<sbx::scenes::static_mesh>(tree, _mesh_ids[fmt::format("tree_1_{}", sbx::math::random::next<std::uint8_t>(1, 4))], tree_submeshes);

      scene.add_component<sbx::scenes::collider>(tree, sbx::scenes::aabb_collider{sbx::math::vector3{-cell_size.x() / 2.0f, 0.0f, -cell_size.y() / 2.0f}, sbx::math::vector3{cell_size.x() / 2.0f, 5.0f, cell_size.y() / 2.0f}});

      const auto position = (sbx::math::vector2{x, y} * cell_size - offset) + (sbx::math::vector2{sbx::math::random::next<std::float_t>(0.0f, 1.0f), sbx::math::random::next<std::float_t>(0.0f, 1.0f)} * cell_size);

      auto& tree_transform = scene.get_component<sbx::math::transform>(tree);
      tree_transform.set_position(sbx::math::vector3{position.x(), 0.0f, position.y()});
      // tree_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});
      tree_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{sbx::math::random::next<std::float_t>(0.0f, 360.0f)});

      if (sbx::math::random::next<std::float_t>(0.0f, 1.0f) >= 0.5f) {
        auto bush = scene.create_child_node(forrest, fmt::format("Bush{}{}", x, y));
  
        scene.add_component<sbx::scenes::static_mesh>(bush, _mesh_ids["bush_5"], tree_submeshes);
  
        scene.add_component<sbx::scenes::collider>(bush, sbx::scenes::aabb_collider{sbx::math::vector3{-cell_size.x() / 2.0f, 0.0f, -cell_size.y() / 2.0f}, sbx::math::vector3{cell_size.x() / 2.0f, 5.0f, cell_size.y() / 2.0f}});
  
        const auto bush_position = (sbx::math::vector2{x, y} * cell_size - offset) + (sbx::math::vector2{sbx::math::random::next<std::float_t>(0.0f, 1.0f), sbx::math::random::next<std::float_t>(0.0f, 1.0f)} * cell_size);
  
        auto& bush_transform = scene.get_component<sbx::math::transform>(bush);
        bush_transform.set_position(sbx::math::vector3{bush_position.x(), 0.0f, bush_position.y()});
        // bush_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});
        bush_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{sbx::math::random::next<std::float_t>(0.0f, 360.0f)});
      }
    }
  }

  auto test = scene.create_node("Test");
  auto& test_transform = scene.get_component<sbx::math::transform>(test);
  test_transform.set_position(sbx::math::vector3{15.0f, 5.0f, 0.0f});
  scene.add_component<sbx::scenes::static_mesh>(test, _mesh_ids["sphere"], 0u, true, sbx::math::color::red(), sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f}, _texture_ids["tree_1_leaves1"]);
  scene.add_component<sbx::scenes::collider>(test, sbx::scenes::sphere_collider{sbx::math::vector3::zero, 1.0f});

  auto test2 = scene.create_node("Test2");
  auto& test2_transform = scene.get_component<sbx::math::transform>(test2);
  test2_transform.set_position(sbx::math::vector3{15.0f, 5.0f, 0.0f});
  test2_transform.set_scale(sbx::math::vector3{4.0f, 4.0f, 4.0f});
  scene.add_component<sbx::scenes::static_mesh>(test2, _mesh_ids["tree_1_2"], 2u, true, sbx::math::color::red(), sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f}, _texture_ids["tree_1_leaves2"]);
  scene.add_component<sbx::scenes::collider>(test2, sbx::scenes::sphere_collider{sbx::math::vector3::zero, 1.0f});

  // Camera
  auto camera = scene.camera();

  scene.add_component<sbx::scenes::skybox>(camera, _texture_ids["skybox"], sbx::math::color::white());

  // const auto position = sbx::math::vector3{10.0f, 10.0f, 10.0f};

  // camera.get_component<sbx::math::transform>().set_position(position);
  // camera.get_component<sbx::math::transform>().look_at(sbx::math::vector3::zero);

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
