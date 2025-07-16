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

#include <libsbx/scenes/debug_subrenderer.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/skinned_mesh.hpp>

#include <libsbx/animations/mesh.hpp>
#include <libsbx/animations/animation.hpp>

namespace demo {

struct rotator { };

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

    const auto id = graphics_module.add_resource<sbx::graphics::image2d>(path);

    _image_ids.emplace(name, id);
  }

  _image_ids.emplace("tree_1_leaves1", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/tree_1/leaves1.png"));
  _image_ids.emplace("tree_1_leaves2", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/tree_1/leaves2.png"));
  _image_ids.emplace("tree_1_bark", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/tree_1/bark.jpg"));

  _image_ids.emplace("maple_tree_bark", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/maple_tree/bark.png"));
  _image_ids.emplace("maple_tree_bark_normal", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/maple_tree/bark_normal.png"));
  _image_ids.emplace("maple_tree_leaves", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/maple_tree/leaves.png"));
  
  _image_ids.emplace("rocks", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/rocks/rocks.png"));

  _image_ids.emplace("fox", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/fox/albedo.png"));

  _image_ids.emplace("women", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/women/albedo.png"));

  _cube_image_ids.emplace("skybox", graphics_module.add_resource<sbx::graphics::cube_image>("demo/assets/skyboxes/clouds"));

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

  _mesh_ids.emplace("maple_tree_1", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/maple_tree_1/maple_tree_1.gltf"));
  _mesh_ids.emplace("maple_tree_2", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/maple_tree_2/maple_tree_2.gltf"));
  _mesh_ids.emplace("maple_tree_3", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/maple_tree_3/maple_tree_3.gltf"));
  _mesh_ids.emplace("maple_tree_4", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/maple_tree_4/maple_tree_4.gltf"));

  _mesh_ids.emplace("rock_1", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_1/rock_1.gltf"));
  _mesh_ids.emplace("rock_2", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_2/rock_2.gltf"));
  _mesh_ids.emplace("rock_3", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_3/rock_3.gltf"));
  _mesh_ids.emplace("rock_4", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_4/rock_4.gltf"));
  _mesh_ids.emplace("rock_5", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_5/rock_5.gltf"));

  _mesh_ids.emplace("player", graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/player/player.gltf"));

  _mesh_ids.emplace("fox", graphics_module.add_asset<sbx::animations::mesh>("demo/assets/meshes/fox/fox.gltf"));

  _mesh_ids.emplace("women", graphics_module.add_asset<sbx::animations::mesh>("demo/assets/meshes/women/women.gltf"));

  const auto fox_animation_id = graphics_module.add_asset<sbx::animations::animation>("demo/assets/meshes/fox/fox.gltf");
  const auto women_animation_id = graphics_module.add_asset<sbx::animations::animation>("demo/assets/meshes/women/women.gltf");

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

  // Player

  _player = scene.create_node("Player");

  auto player_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  player_submeshes.emplace_back(sbx::scenes::static_mesh::submesh{0u, sbx::math::color::red(), sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f}});

  scene.add_component<sbx::scenes::static_mesh>(_player, _mesh_ids["player"], player_submeshes);

  auto& player_transform = scene.get_component<sbx::math::transform>(_player);
  player_transform.set_position(sbx::math::vector3{0.0f, 1.0f, -4.0f});
  player_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // // Camera

  // auto camera = scene.create_child_node(_player, "Camera");

  // scene.add_component<sbx::scenes::camera>(camera, sbx::math::angle{sbx::math::degree{50.0f}}, window.aspect_ratio(), 0.1f, 1000.0f);

  // scene.add_component<sbx::scenes::skybox>(camera, _cube_image_ids["skybox"]);

  // scene.set_active_camera(camera);

  // Animated Fox

  // auto fox = scene.create_node("Fox");

  // auto fox_submeshes = std::vector<sbx::scenes::skinned_mesh::submesh>{};
  // fox_submeshes.push_back(sbx::scenes::skinned_mesh::submesh{0u, sbx::math::color::white(), sbx::scenes::skinned_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, _image_ids["fox"]});

  // scene.add_component<sbx::scenes::skinned_mesh>(fox, _mesh_ids["fox"], fox_animation_id, fox_submeshes);

  // scene.add_component<sbx::scenes::animation_state>(fox);

  // auto& fox_transform = scene.get_component<sbx::math::transform>(fox);
  // fox_transform.set_position(sbx::math::vector3{0.0f, 10.0f, 0.0f});
  // fox_transform.set_scale(sbx::math::vector3{0.1f, 0.1f, 0.1f});

  auto women = scene.create_node("Women");

  auto women_submeshes = std::vector<sbx::scenes::skinned_mesh::submesh>{};
  women_submeshes.push_back(sbx::scenes::skinned_mesh::submesh{0u, sbx::math::color::white(), sbx::scenes::skinned_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, _image_ids["women"]});

  scene.add_component<sbx::scenes::skinned_mesh>(women, _mesh_ids["women"], women_animation_id, women_submeshes);

  scene.add_component<sbx::scenes::animation_state>(women);

  auto& women_transform = scene.get_component<sbx::math::transform>(women);
  women_transform.set_position(sbx::math::vector3{0.0f, 2.0f, 5.0f});
  women_transform.set_scale(sbx::math::vector3{0.1f, 0.1f, 0.1f});

  // Trees

  auto tree_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{0u, sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, _image_ids["maple_tree_bark"], _image_ids["maple_tree_bark_normal"]});
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{1u, sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.0f}, _image_ids["maple_tree_leaves"]});

  // auto maple_tree = scene.create_node("MapleTree");
  // scene.add_component<sbx::scenes::static_mesh>(maple_tree, _mesh_ids["maple_tree_4"], tree_submeshes);

  const auto grid_size = sbx::math::vector2{15.0f, 15.0f};
  const auto cell_size = sbx::math::vector2{15.0f, 15.0f};
  const auto offset = grid_size * cell_size * 0.5f;

  auto forrest = scene.create_node("Forrest");

  for (auto y : std::views::iota(0u, grid_size.y())) {
    for (auto x : std::views::iota(0u, grid_size.x())) {
      auto tree = scene.create_child_node(forrest, fmt::format("Tree{}{}", x, y));

      scene.add_component<sbx::scenes::static_mesh>(tree, _mesh_ids[fmt::format("maple_tree_{}", sbx::math::random::next<std::uint32_t>(1, 4))], tree_submeshes);

      const auto position = (sbx::math::vector2{x, y} * cell_size - offset) + (sbx::math::vector2{sbx::math::random::next<std::float_t>(0.0f, 1.0f), sbx::math::random::next<std::float_t>(0.0f, 1.0f)} * cell_size);

      auto& tree_transform = scene.get_component<sbx::math::transform>(tree);
      tree_transform.set_position(sbx::math::vector3{position.x(), 0.0f, position.y()});
      // tree_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});
      tree_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{sbx::math::random::next<std::float_t>(0.0f, 360.0f)});
    }
  }

  // Rocks

  auto rock_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  rock_submeshes.push_back(sbx::scenes::static_mesh::submesh{0u, sbx::math::color::white(), sbx::scenes::static_mesh::material{0.2f, 0.5f, 0.1f, 0.8f}, _image_ids["rocks"]});

  auto rock_1 = scene.create_node("Rock1");
  scene.add_component<sbx::scenes::static_mesh>(rock_1, _mesh_ids["rock_1"], rock_submeshes);
  auto& rock_1_transform = scene.get_component<sbx::math::transform>(rock_1);
  rock_1_transform.set_position(sbx::math::vector3{-6.0f, 0.0f, 0.0f});
  rock_1_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  auto rock_2 = scene.create_node("Rock2");
  scene.add_component<sbx::scenes::static_mesh>(rock_2, _mesh_ids["rock_2"], rock_submeshes);
  auto& rock_2_transform = scene.get_component<sbx::math::transform>(rock_2);
  rock_2_transform.set_position(sbx::math::vector3{-3.0f, 0.0f, 0.0f});
  rock_2_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  auto rock_3 = scene.create_node("Rock3");
  scene.add_component<sbx::scenes::static_mesh>(rock_3, _mesh_ids["rock_3"], rock_submeshes);
  auto& rock_3_transform = scene.get_component<sbx::math::transform>(rock_3);
  rock_3_transform.set_position(sbx::math::vector3{0.0f, 0.0f, 0.0f});
  rock_3_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  auto rock_4 = scene.create_node("Rock4");
  scene.add_component<sbx::scenes::static_mesh>(rock_4, _mesh_ids["rock_4"], rock_submeshes);
  auto& rock_4_transform = scene.get_component<sbx::math::transform>(rock_4);
  rock_4_transform.set_position(sbx::math::vector3{3.0f, 0.0f, 0.0f});
  rock_4_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  auto rock_5 = scene.create_node("Rock5");
  scene.add_component<sbx::scenes::static_mesh>(rock_5, _mesh_ids["rock_5"], rock_submeshes);
  auto& rock_5_transform = scene.get_component<sbx::math::transform>(rock_5);
  rock_5_transform.set_position(sbx::math::vector3{6.0f, 0.0f, 0.0f});
  rock_5_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // Cubes

  auto height_map = std::vector<std::float_t>{};
  height_map.resize(100 * 100);

  for (auto y = 0; y < 100; ++y) {
    for (auto x = 0; x < 100; ++x) {
      height_map[x + y * 100] = sbx::math::noise::fractal(x * 0.05f, y * 0.05f, 5);
    }
  }

  _image_ids.emplace("height_map", graphics_module.add_resource<sbx::graphics::image2d>(sbx::math::vector2u{100, 100}, VK_FORMAT_R32_SFLOAT, reinterpret_cast<const std::uint8_t*>(height_map.data())));

  // for (auto y = -3; y <= 3; y = y + 3) {
  //   for (auto x = -3; x <= 3; x = x + 3) {
  //     auto test = scene.create_node("Test");
  //     auto& test_transform = scene.get_component<sbx::math::transform>(test);
  //     test_transform.set_position(sbx::math::vector3{x, sbx::math::random::next<std::float_t>(6.0f, 8.0f), y});
  //     test_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});
  //     test_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{45});
  //     scene.add_component<rotator>(test);
  //     // scene.add_component<sbx::scenes::static_mesh>(test, _mesh_ids["sphere"], 0u, sbx::math::color{1.0f, 0.6f, 0.6f, 1.0f}, sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f});
  //     scene.add_component<sbx::scenes::static_mesh>(test, _mesh_ids["cube"], 0u, sbx::math::color{0.39f, 0.44f, 0.56f, 1.0f}, sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f}, _image_ids["height_map"]);
  //   }
  // }

  // Camera
  auto camera = scene.camera();

  scene.add_component<sbx::scenes::skybox>(camera, _cube_image_ids["skybox"]);

  // const auto position = sbx::math::vector3{10.0f, 10.0f, 10.0f};

  // camera.get_component<sbx::math::transform>().set_position(position);
  // camera.get_component<sbx::math::transform>().look_at(sbx::math::vector3::zero);

  const auto& cli = sbx::core::engine::cli();

  if (auto hide_window = cli.argument<bool>("hide-window"); !hide_window) {
    window.show();
  }
}

auto application::update() -> void  {
  if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
    sbx::core::engine::quit();
    return;
  }

  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  auto& scene = scenes_module.scene();

  _camera_controller.update();
    
  const auto delta_time = sbx::core::engine::delta_time();

  _rotation += sbx::math::degree{45} * delta_time;

  auto query_rotator = scene.query<sbx::math::transform, rotator>();

  for (auto&& [node, transform] : query_rotator.each()) {
    transform.set_rotation(sbx::math::vector3::up, _rotation);
  }
}

auto application::fixed_update() -> void {

}

} // namespace demo
