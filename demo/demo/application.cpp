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

#include <libsbx/devices/input.hpp>

#include <libsbx/scenes/debug_subrenderer.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/skinned_mesh.hpp>

#include <libsbx/animations/mesh.hpp>
#include <libsbx/animations/animation.hpp>

namespace demo {

struct rotator { };

struct walker { };

application::application()
: sbx::core::application{},
  _rotation{sbx::math::degree{0}} { 
  // Renderer

  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  graphics_module.set_renderer<renderer>();

  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  // Textures

  _image_ids.emplace("maple_tree_bark", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/maple_tree/bark.png"));
  _image_ids.emplace("maple_tree_bark_normal", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/maple_tree/bark_normal.png"));
  _image_ids.emplace("maple_tree_leaves", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/maple_tree/leaves.png"));
  
  _image_ids.emplace("rocks", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/rocks/rocks.png"));

  _image_ids.emplace("fox", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/fox/albedo.png"));

  _image_ids.emplace("women", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/women/albedo.png"));

  _cube_image_ids.emplace("skybox", graphics_module.add_resource<sbx::graphics::cube_image>("demo/assets/skyboxes/clouds"));

  _image_ids.emplace("bmp_body1_albedo", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/bmp/body1_albedo.png"));
  _image_ids.emplace("bmp_body1_normal", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/bmp/body1_normal.png"));
  _image_ids.emplace("bmp_body2_albedo", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/bmp/body2_albedo.png"));
  _image_ids.emplace("bmp_body2_normal", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/bmp/body2_normal.png"));
  _image_ids.emplace("bmp_tracks_albedo", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/bmp/tracks_albedo.png"));
  _image_ids.emplace("bmp_tracks_normal", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/bmp/tracks_normal.png"));

  _image_ids.emplace("cerberus_albedo", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/cerberus/albedo.png"));
  _image_ids.emplace("cerberus_normal", graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/cerberus/normal.png"));

  // Meshes

  _mesh_ids.emplace("maple_tree_1", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/maple_tree_1/maple_tree_1.gltf"));
  _mesh_ids.emplace("maple_tree_2", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/maple_tree_2/maple_tree_2.gltf"));
  _mesh_ids.emplace("maple_tree_3", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/maple_tree_3/maple_tree_3.gltf"));
  _mesh_ids.emplace("maple_tree_4", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/maple_tree_4/maple_tree_4.gltf"));

  _mesh_ids.emplace("rock_1", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_1/rock_1.gltf"));
  _mesh_ids.emplace("rock_2", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_2/rock_2.gltf"));
  _mesh_ids.emplace("rock_3", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_3/rock_3.gltf"));
  _mesh_ids.emplace("rock_4", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_4/rock_4.gltf"));
  _mesh_ids.emplace("rock_5", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/rock_5/rock_5.gltf"));

  _mesh_ids.emplace("fox", assets_module.add_asset<sbx::animations::mesh>("demo/assets/meshes/fox/fox.gltf"));
  _mesh_ids.emplace("fox_static", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/fox/fox.gltf"));

  _mesh_ids.emplace("women", assets_module.add_asset<sbx::animations::mesh>("demo/assets/meshes/women/women.gltf"));
  _mesh_ids.emplace("women_static", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/women/women.gltf"));

  // _mesh_ids.emplace("man", assets_module.add_asset<sbx::animations::mesh>("demo/assets/meshes/man/man.gltf"));
  _mesh_ids.emplace("man_static", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/man/man.gltf"));

  const auto fox_animation_id = assets_module.add_asset<sbx::animations::animation>("demo/assets/meshes/fox/fox.gltf", "Walk");
  const auto women_animation_id = assets_module.add_asset<sbx::animations::animation>("demo/assets/meshes/women/women.gltf", "Walking");

  _mesh_ids.emplace("bmp", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/tank/bmp.gltf"));

  _mesh_ids.emplace("cube", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/cube/cube.gltf"));

  // _mesh_ids.emplace("cerberus", assets_module.add_asset<sbx::models::mesh>("demo/assets/meshes/cerberus/cerberus.fbx"));

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

  // auto& terrain_module = sbx::core::engine::get_module<demo::terrain_module>();

  // terrain_module.load_terrain_in_scene(scene);

  // Animated Fox

  auto fox1 = scene.create_node("Fox");

  auto fox1_submeshes = std::vector<sbx::scenes::skinned_mesh::submesh>{};
  fox1_submeshes.push_back(sbx::scenes::skinned_mesh::submesh{0u, sbx::math::color::white(), sbx::scenes::skinned_mesh::material{}, _image_ids["fox"]});

  scene.add_component<sbx::scenes::skinned_mesh>(fox1, _mesh_ids["fox"], fox_animation_id, fox1_submeshes);

  scene.add_component<sbx::scenes::animation_state>(fox1);

  scene.add_component<walker>(fox1);


  auto& fox1_transform = scene.get_component<sbx::math::transform>(fox1);
  fox1_transform.set_position(sbx::math::vector3{0.0f, 0.0f, 0.0f});
  fox1_transform.set_scale(sbx::math::vector3{0.06f, 0.06f, 0.06f});

  _selection_buffer = graphics_module.add_resource<sbx::graphics::storage_buffer>(sbx::graphics::storage_buffer::min_size);

  // Tank

  _tanks.emplace_back(sbx::math::transform{sbx::math::vector3::zero, sbx::math::quaternion::identity, sbx::math::vector3{0.5}}, _mesh_ids, _image_ids);

  // Trees

  auto tree_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{0u, sbx::math::color::white(), sbx::scenes::static_mesh::material{}, _image_ids["maple_tree_bark"], _image_ids["maple_tree_bark_normal"]});
  tree_submeshes.push_back(sbx::scenes::static_mesh::submesh{1u, sbx::math::color::white(), sbx::scenes::static_mesh::material{}, _image_ids["maple_tree_leaves"]});

  auto maple_tree = scene.create_node("MapleTree");
  scene.add_component<sbx::scenes::static_mesh>(maple_tree, _mesh_ids["maple_tree_4"], tree_submeshes);

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
  rock_submeshes.push_back(sbx::scenes::static_mesh::submesh{0u, sbx::math::color::white(), sbx::scenes::static_mesh::material{}, _image_ids["rocks"]});

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

  for (auto y = -3; y <= 3; y = y + 3) {
    for (auto x = -3; x <= 3; x = x + 3) {
      auto test = scene.create_node("Test");
      auto& test_transform = scene.get_component<sbx::math::transform>(test);
      test_transform.set_position(sbx::math::vector3{x, sbx::math::random::next<std::float_t>(6.0f, 8.0f), y});
      test_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});
      test_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{45});
      scene.add_component<rotator>(test);
      // scene.add_component<sbx::scenes::static_mesh>(test, _mesh_ids["sphere"], 0u, sbx::math::color{1.0f, 0.6f, 0.6f, 1.0f}, sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f});
      scene.add_component<sbx::scenes::static_mesh>(test, _mesh_ids["cube"], 0u, sbx::math::color{0.39f, 0.44f, 0.56f, 1.0f}, sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f}, _image_ids["height_map"]);
    }
  }

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

  sbx::utility::logger<"demo">::info("string id: {}", sbx::utility::string_id<"foobar">());

  scenes_module.save_scene("demo/assets/scenes/test.yaml");
}

// [NOTE] : This might or might not me a great thing :D
static auto select_object_ids_in_rect(const sbx::graphics::image2d& image, VkFormat format, sbx::graphics::storage_buffer& buffer, int x0, int y0, int width, int height) -> std::unordered_set<uint64_t> {
  const auto buffer_size = width * height * sizeof(std::uint32_t) * 2;

  if (buffer.size() < buffer_size) {
    buffer.resize(static_cast<std::size_t>(static_cast<std::float_t>(buffer_size) * 1.5f));
  }

  VkOffset3D offset = { x0, y0, 0 };
  VkExtent3D extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
  sbx::graphics::image::copy_image_to_buffer(image, format, buffer, offset, extent, 1, 0);

  std::unordered_set<std::uint64_t> ids;

  for (uint32_t i = 0; i < width * height; ++i) {
    std::uint64_t id = (std::uint64_t(buffer.read<std::uint32_t>(i * 2 + 0)) << 32) | std::uint64_t(buffer.read<std::uint32_t>(i * 2 + 1));
    
    if (id != 0) {
      ids.insert(id);
    }
  }

  return ids;
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

  auto query_walker = scene.query<sbx::math::transform, walker>();

  for (auto&& [node, transform] : query_walker.each()) {
    transform.move_by(transform.forward() * delta_time * 2.0f);
  }

  for (auto& tank : _tanks) {
    tank.update();
  }

  // const auto& image = static_cast<const sbx::graphics::image2d&>(graphics_module.attachment("object_id"));
  // auto& buffer = graphics_module.get_resource<sbx::graphics::storage_buffer>(_selection_buffer);

  // static auto start = sbx::math::vector2u{0, 0};

  // auto ids = select_object_ids_in_rect(image, VK_FORMAT_R32G32_UINT, buffer, 100, 100, 300, 300);

  // for (const auto id : ids) {
  //   sbx::utility::logger<"demo">::info("object id: {}", id);
  // }

  // sbx::utility::logger<"demo">::info("object id: {}", sbx::devices::input::mouse_position());
}

auto application::fixed_update() -> void {

}

} // namespace demo
