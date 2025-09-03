#include <demo/application.hpp>

#include <nlohmann/json.hpp>

#include <easy/profiler.h>

#include <libsbx/math/color.hpp>
#include <libsbx/math/noise.hpp>
#include <libsbx/math/constants.hpp>

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

#include <libsbx/scripting/scripting.hpp>

#include <libsbx/animations/mesh.hpp>
#include <libsbx/animations/animation.hpp>
#include <libsbx/animations/animator.hpp>

namespace demo {

struct rotator { };

struct walker { };

struct show_local_coordinates { };

static auto fox1 = sbx::scenes::node{};

application::application()
: sbx::core::application{},
  _rotation{sbx::math::degree{0}} { 
  // Renderer
  const auto& cli = sbx::core::engine::cli();

  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  if (auto assets = cli.argument<std::string>("assets"); assets) {
    assets_module.set_asset_root(*assets);
  } else {
    assets_module.set_asset_root("demo/assets");
  }

  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  graphics_module.set_renderer<renderer>();

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.load_scene("res://scenes/scene.yaml");

  auto& scripting_module = sbx::core::engine::get_module<sbx::scripting::scripting_module>();

  scripting_module.load_domain();

  scripting_module.load_assembly("Sbx", "res://scripts/Sbx/Out/Sbx.dll");

  scripting_module.load_assembly("Test", "res://scripts/Out/Test.dll");

  // Textures

  scene.add_image("maple_tree_bark", "res://textures/maple_tree/bark.png");
  scene.add_image("maple_tree_bark_normal", "res://textures/maple_tree/bark_normal.png");
  scene.add_image("maple_tree_leaves", "res://textures/maple_tree/leaves.png");
  
  scene.add_image("rocks", "res://textures/rocks/rocks.png");

  scene.add_image("fox_albedo", "res://textures/fox/albedo.png");

  scene.add_image("women", "res://textures/women/albedo.png");

  scene.add_image("bmp_body1_albedo", "res://textures/bmp/body1_albedo.png");
  scene.add_image("bmp_body1_normal", "res://textures/bmp/body1_normal.png");
  scene.add_image("bmp_body2_albedo", "res://textures/bmp/body2_albedo.png");
  scene.add_image("bmp_body2_normal", "res://textures/bmp/body2_normal.png");
  scene.add_image("bmp_tracks_albedo", "res://textures/bmp/tracks_albedo.png");
  scene.add_image("bmp_tracks_normal", "res://textures/bmp/tracks_normal.png");

  scene.add_image("helmet_albedo", "res://textures/helmet/albedo.jpg");
  scene.add_image("helmet_normal", "res://textures/helmet/normal.jpg");
  scene.add_image("helmet_mrao", "res://textures/helmet/mrao.jpg");

  scene.add_image("grass3_albedo", "res://textures/grass3/albedo.png");
  scene.add_image("grass3_normal", "res://textures/grass3/normal.png");
  scene.add_image("grass3_mrao", "res://textures/grass3/mrao.png");

  scene.add_image("checkerboard", "res://textures/checkerboard.jpg");

  scene.add_image("soldier_body_albedo", "res://textures/soldier/body_albedo.png");
  scene.add_image("soldier_head_albedo", "res://textures/soldier/head_albedo.png");
  scene.add_image("soldier_backpack_albedo", "res://textures/soldier/backpack_albedo.png");
  scene.add_image("soldier_helmet_albedo", "res://textures/soldier/helmet_albedo.png");

  scene.add_image("rust_albedo", "res://textures/rust/albedo.png");
  scene.add_image("rust_normal", "res://textures/rust/normal.png");
  scene.add_image("rust_mrao", "res://textures/rust/mrao.png");

  scene.add_cube_image("skybox", "res://skyboxes/stylized2");

  // Meshes

  scene.add_mesh<sbx::models::mesh>("maple_tree_1", "res://meshes/maple_tree_1/maple_tree_1.gltf");
  scene.add_mesh<sbx::models::mesh>("maple_tree_2", "res://meshes/maple_tree_2/maple_tree_2.gltf");
  scene.add_mesh<sbx::models::mesh>("maple_tree_3", "res://meshes/maple_tree_3/maple_tree_3.gltf");
  scene.add_mesh<sbx::models::mesh>("maple_tree_4", "res://meshes/maple_tree_4/maple_tree_4.gltf");

  scene.add_mesh<sbx::animations::mesh>("fox", "res://meshes/fox/fox.gltf");
  scene.add_mesh<sbx::models::mesh>("fox_static", "res://meshes/fox/fox.gltf");

  scene.add_mesh<sbx::animations::mesh>("women", "res://meshes/women/women.gltf");
  scene.add_mesh<sbx::models::mesh>("women_static", "res://meshes/women/women.gltf");

  scene.add_mesh<sbx::animations::mesh>("player", "res://meshes/player/player.gltf");
  
  scene.add_mesh<sbx::models::mesh>("man_static", "res://meshes/man/man.gltf");

  scene.add_mesh<sbx::models::mesh>("bmp", "res://meshes/tank/bmp.gltf");

  scene.add_mesh<sbx::models::mesh>("helmet", "res://meshes/helmet/helmet.gltf");

  scene.add_mesh<sbx::models::mesh>("dragon", "res://meshes/dragon/dragon.gltf");

  scene.add_mesh<sbx::models::mesh>("cube", "res://meshes/cube/cube.gltf");
  scene.add_mesh<sbx::models::mesh>("sphere", "res://meshes/sphere/sphere.gltf");

  scene.add_mesh<sbx::animations::mesh>("soldier", "res://meshes/soldier/soldier.gltf");
  scene.add_mesh<sbx::models::mesh>("soldier_static", "res://meshes/soldier/soldier.gltf");

  // Animations

  scene.add_animation<sbx::animations::animation>("Walk", "res://meshes/fox/fox.gltf", "Walk");
  scene.add_animation<sbx::animations::animation>("Survey", "res://meshes/fox/fox.gltf", "Survey");
  scene.add_animation<sbx::animations::animation>("Run", "res://meshes/fox/fox.gltf", "Run");

  scene.add_animation<sbx::animations::animation>("Walking", "res://meshes/women/women.gltf", "Walking");
  scene.add_animation<sbx::animations::animation>("IdleStanding", "res://meshes/soldier/soldier.gltf", "IdleStanding");
  scene.add_animation<sbx::animations::animation>("ArmatureAction", "res://meshes/player/player.gltf", "ArmatureAction");

  // Window

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  // Terrain

  auto& terrain_module = sbx::core::engine::get_module<demo::terrain_module>();

  terrain_module.load_terrain_in_scene();

  // Soldier

  // scene.add_material<sbx::scenes::material>("player", sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.0f, 0.5f, 1.0f);

  // auto player = scene.create_node("Player");

  // const auto player_submeshes = std::vector<sbx::scenes::skinned_mesh::submesh>{
  //   {0u, scene.get_material("player")},
  //   {1u, scene.get_material("player")},
  //   {2u, scene.get_material("player")}
  // };

  // scene.add_component<sbx::scenes::skinned_mesh>(player, scene.get_mesh("player"), player_animation_id, player_submeshes);
  // scene.add_component<sbx::scenes::animation_state>(player, 0.0f, 1.0f, true);
  
  scene.add_material<sbx::scenes::material>("soldier_body", sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.0f, 0.9f, 1.0f, scene.get_image("soldier_body_albedo"));
  scene.add_material<sbx::scenes::material>("soldier_head", sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.0f, 0.9f, 1.0f, scene.get_image("soldier_head_albedo"));
  scene.add_material<sbx::scenes::material>("soldier_backpack", sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.0f, 0.9f, 1.0f, scene.get_image("soldier_backpack_albedo"));
  scene.add_material<sbx::scenes::material>("soldier_helmet", sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.0f, 0.9f, 1.0f, scene.get_image("soldier_helmet_albedo"));
  
  auto soldier_submeshes = std::vector<sbx::scenes::skinned_mesh::submesh>{
    {0u, scene.get_material("soldier_body")},
    {1u, scene.get_material("soldier_head")},
    {2u, scene.get_material("soldier_helmet")},
    {3u, scene.get_material("soldier_backpack")}
  };

  auto soldier = scene.create_node("Soldier");

  scene.add_component<show_local_coordinates>(soldier);

  scene.add_component<sbx::scenes::skinned_mesh>(soldier, scene.get_mesh("soldier"), scene.get_animation("IdleStanding"), soldier_submeshes);

  auto& soldier_animator = scene.add_component<sbx::animations::animator>(soldier);
  soldier_animator.add_state({"IdleStanding", scene.get_animation("IdleStanding"), true, 1.0f});

  soldier_animator.play("IdleStanding", true);

  auto& soldier_transform = scene.get_component<sbx::scenes::transform>(soldier);
  soldier_transform.set_position(sbx::math::vector3{5.0f, 0.0f, 3.0f});
  soldier_transform.set_scale(sbx::math::vector3{3.0f});

  // auto soldier = scene.create_node("Soldier");

  // auto soldier_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{
  //   {0u, scene.get_material("soldier_body")},
  //   {1u, scene.get_material("soldier_head")},
  //   {2u, scene.get_material("soldier_helmet")},
  //   {3u, scene.get_material("soldier_backpack")}
  // };

  // scene.add_component<sbx::scenes::static_mesh>(soldier, scene.get_mesh("soldier_static"), soldier_submeshes);

  // auto& soldier_transform = scene.get_component<sbx::scenes::transform>(soldier);
  // soldier_transform.set_position(sbx::math::vector3{5.0f, 0.0f, 3.0f});
  // soldier_transform.set_scale(sbx::math::vector3{3.0f});

  // auto soldier1 = scene.create_node("Soldier");

  // scene.add_component<sbx::scenes::skinned_mesh>(soldier1, scene.get_mesh("soldier"), soldier_animation_id, std::vector<sbx::scenes::skinned_mesh::submesh>{{0u, scene.get_material("soldier_body")}});  
  // scene.add_component<sbx::scenes::animation_state>(soldier1, 0.0f, 1.0f, true);

  // auto& soldier_transform = scene.get_component<sbx::scenes::transform>(soldier1);
  // soldier_transform.set_position(sbx::math::vector3{7, 0, 3});
  // soldier_transform.set_scale(sbx::math::vector3{4, 4, 4});

  // auto soldier2 = scene.create_node("Soldier2");

  // scene.add_component<sbx::scenes::skinned_mesh>(soldier2, scene.get_mesh("soldier"), soldier_animation_id, std::vector<sbx::scenes::skinned_mesh::submesh>{{1u, scene.get_material("soldier_helmet")}});  
  // scene.add_component<sbx::scenes::animation_state>(soldier2, 0.0f, 1.0f, true);

  // auto& soldier2_transform = scene.get_component<sbx::scenes::transform>(soldier2);
  // soldier2_transform.set_position(sbx::math::vector3{10, 0, 3});
  // soldier2_transform.set_scale(sbx::math::vector3{4, 4, 4});

  // auto soldier3 = scene.create_node("Soldier3");

  // scene.add_component<sbx::scenes::skinned_mesh>(soldier3, scene.get_mesh("soldier"), soldier_animation_id, std::vector<sbx::scenes::skinned_mesh::submesh>{{2u, scene.get_material("soldier_backpack")}});  
  // scene.add_component<sbx::scenes::animation_state>(soldier3, 0.0f, 1.0f, true);

  // auto& soldier3_transform = scene.get_component<sbx::scenes::transform>(soldier3);
  // soldier3_transform.set_position(sbx::math::vector3{13, 0, 3});
  // soldier3_transform.set_scale(sbx::math::vector3{4, 4, 4});

  // auto soldier4 = scene.create_node("Soldier4");

  // scene.add_component<sbx::scenes::skinned_mesh>(soldier4, scene.get_mesh("soldier"), soldier_animation_id, std::vector<sbx::scenes::skinned_mesh::submesh>{{3u, scene.get_material("soldier_head")}});  
  // scene.add_component<sbx::scenes::animation_state>(soldier4, 0.0f, 1.0f, true);

  // auto& soldier4_transform = scene.get_component<sbx::scenes::transform>(soldier4);
  // soldier4_transform.set_position(sbx::math::vector3{16, 0, 3});
  // soldier4_transform.set_scale(sbx::math::vector3{4, 4, 4});

  // auto soldier_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{
  //   {0u, scene.get_material("soldier_body")},
  //   {1u, scene.get_material("soldier_head")},
  //   {2u, scene.get_material("soldier_helmet")},
  //   {3u, scene.get_material("soldier_backpack")}
  // };

  // scene.add_component<sbx::scenes::static_mesh>(soldier, scene.get_mesh("soldier"), soldier_submeshes);

  // Circling point lights

  _light_center = scene.create_node("LightCenter", sbx::scenes::transform{sbx::math::vector3{0.0f, 10.0f, 0.0f}});

  
  const auto radius = 20.0f;
  const auto light_count = 8;
  
  for (auto i = 0; i < light_count; ++i) {
    auto angle = sbx::math::radian{2.0f * sbx::math::pi / static_cast<std::float_t>(light_count) * static_cast<std::float_t>(i)};

    const auto material_name = fmt::format("Light{}", i);
    const auto color = sbx::math::random_color();

    scene.add_material<sbx::scenes::material>(material_name, sbx::scenes::material_type::transparent, color, 0.0f, 0.5f, 1.0f);

    auto light = scene.create_child_node(_light_center, fmt::format("Light{}", i), sbx::scenes::transform{sbx::math::vector3{radius * sbx::math::cos(angle), 0.0f, radius * sbx::math::sin(angle)}});

    scene.add_component<sbx::scenes::point_light>(light, color, 50.0f);

    scene.add_component<sbx::scenes::static_mesh>(light, scene.get_mesh("sphere"), scene.get_material(material_name));

    auto& light_transform = scene.get_component<sbx::scenes::transform>(light);
    light_transform.set_scale(sbx::math::vector3{0.2f, 0.2f, 0.2f});
  }

  // Dragon
  auto& dragon_mesh = assets_module.get_asset<sbx::models::mesh>(scene.get_mesh("dragon"));

  auto dragon = scene.create_node("Dragon"); //, sbx::scenes::transform{dragon_mesh.submesh_local_transform("dragon") * sbx::math::vector4{0, 0, 0, 1}});

  scene.add_material<sbx::scenes::material>("cloth", sbx::scenes::material_type::opaque, sbx::math::color::blue(), 0.0f, 1.0f, 1.0f, scene.get_image("checkerboard"));
  scene.add_material<sbx::scenes::material>("dragon", sbx::scenes::material_type::transparent, sbx::math::color{0.0f, 0.6588f, 0.4196f, 0.6f}, 0.0f, 0.5f, 1.0f);

  scene.add_component<sbx::scenes::static_mesh>(dragon, scene.get_mesh("dragon"), std::vector<sbx::scenes::static_mesh::submesh>{{0u, scene.get_material("cloth")}, {1u, scene.get_material("dragon")}});

  auto& dragon_transform = scene.get_component<sbx::scenes::transform>(dragon);
  dragon_transform.set_position(sbx::math::vector3{-8.0f, 2.0f, 4.0f});
  dragon_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{45});
  dragon_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});


  // Helmet
  auto helmet = scene.create_node("Helmet");

  scene.add_material<sbx::scenes::material>("helmet", sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.0f, 0.5f, 1.0f, scene.get_image("helmet_albedo"), scene.get_image("helmet_normal"));

  scene.add_component<show_local_coordinates>(helmet);

  scene.add_component<sbx::scenes::static_mesh>(helmet, scene.get_mesh("helmet"), scene.get_material("helmet"));

  auto& helmet_transform = scene.get_component<sbx::scenes::transform>(helmet);
  helmet_transform.set_position(sbx::math::vector3{0.0f, 6.0f, 0.0f});
  // helmet_transform.set_rotation(sbx::math::vector3::right, sbx::math::degree{90});
  helmet_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});


  // Box1
  auto box1 = scene.create_node("Box1");

  scene.add_material<sbx::scenes::material>("box1", sbx::scenes::material_type::transparent, sbx::math::color{1.0f, 0.0f, 0.0f, 0.5f}, 0.0f, 0.5f, 1.0f);

  scene.add_component<sbx::scenes::static_mesh>(box1, scene.get_mesh("cube"), scene.get_material("box1"));

  auto& box1_transform = scene.get_component<sbx::scenes::transform>(box1);
  box1_transform.set_position(sbx::math::vector3{6.0f, 1.0f, 6.0f});


  // Box2
  auto box2 = scene.create_node("Box2");

  scene.add_material<sbx::scenes::material>("box2", sbx::scenes::material_type::opaque, sbx::math::color{0.0f, 1.0f, 0.0f, 0.5f}, 0.0f, 0.5f, 1.0f);

  scene.add_component<sbx::scenes::static_mesh>(box2, scene.get_mesh("cube"), scene.get_material("box2"));

  auto& box2_transform = scene.get_component<sbx::scenes::transform>(box2);
  box2_transform.set_position(sbx::math::vector3{6.0f, 1.0f, 7.0f});


  // Box3
  auto box3 = scene.create_node("Box3");

  scene.add_material<sbx::scenes::material>("box3", sbx::scenes::material_type::transparent, sbx::math::color{0.0f, 0.0f, 1.0f, 0.5f}, 0.0f, 0.5f, 1.0f);

  scene.add_component<sbx::scenes::static_mesh>(box3, scene.get_mesh("cube"), scene.get_material("box3"));

  auto& box3_transform = scene.get_component<sbx::scenes::transform>(box3);
  box3_transform.set_position(sbx::math::vector3{6.0f, 1.0f, 8.0f});

  _selection_buffer = graphics_module.add_resource<sbx::graphics::storage_buffer>(sbx::graphics::storage_buffer::min_size);


  // Fox
  fox1 = scene.create_node("Fox");

  scene.add_material<sbx::scenes::material>("fox", sbx::scenes::material_type::opaque, sbx::math::color::white(), 1.0f, 0.5f, 1.0f, scene.get_image("fox_albedo"));

  scene.add_component<show_local_coordinates>(fox1);

  scene.add_component<sbx::scenes::skinned_mesh>(fox1, scene.get_mesh("fox"), scene.get_animation("Walk"), scene.get_material("fox"));

  auto& fox_animator = scene.add_component<sbx::animations::animator>(fox1);

  fox_animator.add_state({"Walk", scene.get_animation("Walk"), true, 1.0f });
  fox_animator.add_state({"Survey", scene.get_animation("Survey"), true, 1.0f });
  fox_animator.add_state({"Run", scene.get_animation("Run"), true, 1.0f });

  fox_animator.set_float("speed", 0.0f);   // will be updated every frame

  fox_animator.add_transition({
    "Walk", "Survey", 0.20f,
    [](const sbx::animations::animator& animator){
      if (auto value = animator.float_parameter("speed"); value) {
        return *value <= 0.05f;
      }

      return false;
    }
  });

  fox_animator.add_transition({
    "Run", "Survey", 0.25f,
    [](const sbx::animations::animator& animator){
      if (auto value = animator.float_parameter("speed"); value) {
        return *value <= 0.05f;
      }

      return false;
    }
  });

  // Walk ↔ Run thresholds
  fox_animator.add_transition({
    "Walk", "Run", 0.15f,
    [](const sbx::animations::animator& animator){
      if (auto value = animator.float_parameter("speed"); value) {
        return *value >= 2.0f;
      }

      return false;
    }
  });

  fox_animator.add_transition({
    "Run", "Walk", 0.15f,
    [](const sbx::animations::animator& animator){
      if (auto value = animator.float_parameter("speed"); value) {
        return *value < 2.0f && *value > 0.05f;
      }

      return false;
    }
  });

  // Survey → Walk when starting to move
  fox_animator.add_transition({
    "Survey", "Walk", 0.20f,
    [](const sbx::animations::animator& animator){
      if (auto value = animator.float_parameter("speed"); value) {
        return *value > 0.05f && *value < 2.0f;
      }

      return false;
    }
  });

  fox_animator.play("Survey", true);

  auto& fox1_transform = scene.get_component<sbx::scenes::transform>(fox1);
  fox1_transform.set_position(sbx::math::vector3{0.0f, 0.0f, 0.0f});
  fox1_transform.set_scale(sbx::math::vector3{0.06f, 0.06f, 0.06f});

  _selection_buffer = graphics_module.add_resource<sbx::graphics::storage_buffer>(sbx::graphics::storage_buffer::min_size);

  // Tree
  // scene.add_material<sbx::scenes::material>("maple_tree_bark", sbx::scenes::material_type::opaque, sbx::math::color{1.0f, 1.0f, 1.0f, 0.1f}, 0.0f, 0.5f, 1.0f, scene.get_image("maple_tree_bark"), scene.get_image("maple_tree_bark_normal"));
  // scene.add_material<sbx::scenes::material>("maple_tree_leaves", sbx::scenes::material_type::masked, sbx::math::color::white(), 0.0f, 0.5f, 1.0f, scene.get_image("maple_tree_leaves"));

  // auto maple_tree = scene.create_node("MapleTree");
  // scene.add_component<sbx::scenes::static_mesh>(maple_tree, scene.get_mesh("maple_tree_4"), std::vector<sbx::scenes::static_mesh::submesh>{{0u, scene.get_material("maple_tree_bark")}, {1u, scene.get_material("maple_tree_leaves")}});

  // auto& maple_tree_transform = scene.get_component<sbx::scenes::transform>(maple_tree);
  // maple_tree_transform.set_position(sbx::math::vector3{5.0f, 0.0f, 0.0f});
  // maple_tree_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // Cube

  // auto cube = scene.create_node("Cube");

  scene.add_material<sbx::scenes::material>("grass3", sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.0f, 0.5f, 1.0f, scene.get_image("rust_albedo"), scene.get_image("rust_normal"), scene.get_image("rust_mrao"));

  // scene.add_component<sbx::scenes::static_mesh>(cube, scene.get_mesh("cube"), scene.get_material("grass3"));

  // auto& cube_transform = scene.get_component<sbx::scenes::transform>(cube);
  // cube_transform.set_position(sbx::math::vector3{0.0f, 5.0f, 5.0f});
  // cube_transform.set_rotation(sbx::math::vector3::right, sbx::math::degree{45});
  // cube_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // auto& cube_rigidbody = scene.add_component<sbx::physics::rigidbody>(cube, sbx::units::kilogram{1.0f});
  // cube_rigidbody.set_constant_acceleration(sbx::math::vector3{0.0f, -9.81f, 0.0f});

  // scene.add_component<sbx::physics::collider>(cube, sbx::physics::box{sbx::math::vector3{-1.0f, -1.0f, -1.0f}, sbx::math::vector3{1.0f, 1.0f, 1.0f}});

  auto floor = scene.create_node("Floor");
  scene.add_component<sbx::physics::rigidbody>(floor);
  scene.add_component<sbx::physics::collider>(floor, sbx::physics::box{sbx::math::vector3{50.0f, 1.0f, 50.0f}});

  for (auto y = 0; y < 5; ++y) {
    for (auto x = 0; x < 5; ++x) {
      auto sphere = scene.create_node(fmt::format("Sphere{}{}", x, y));

      const auto material_name = fmt::format("sphere_{}_{}_material", x, y);

      scene.add_material<sbx::scenes::material>(material_name, sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.2f * x, 0.2 * y, 1.0f, scene.get_image("checkerboard"));

      scene.add_component<sbx::scenes::static_mesh>(sphere, scene.get_mesh("sphere"), scene.get_material(material_name));

      auto& sphere_transform = scene.get_component<sbx::scenes::transform>(sphere);
      sphere_transform.set_position(sbx::math::vector3{x * 3, y * 3 + 5, -15.0f});
      sphere_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});
    }
  }

  // Tank

  // _tanks.emplace_back(sbx::scenes::transform{sbx::math::vector3::zero, sbx::math::quaternion::identity, sbx::math::vector3{0.5}});

  // const auto grid_size = sbx::math::vector2{15.0f, 15.0f};
  // const auto cell_size = sbx::math::vector2{15.0f, 15.0f};
  // const auto offset = grid_size * cell_size * 0.5f;

  // auto forrest = scene.create_node("Forrest");

  // for (auto y : std::views::iota(0u, grid_size.y())) {
  //   for (auto x : std::views::iota(0u, grid_size.x())) {
  //     auto tree = scene.create_child_node(forrest, fmt::format("Tree{}{}", x, y));

  //     scene.add_component<sbx::scenes::static_mesh>(tree, scene.get_mesh(fmt::format("maple_tree_{}", sbx::math::random::next<std::uint32_t>(1, 4))), tree_submeshes);

  //     const auto position = (sbx::math::vector2{x, y} * cell_size - offset) + (sbx::math::vector2{sbx::math::random::next<std::float_t>(0.0f, 1.0f), sbx::math::random::next<std::float_t>(0.0f, 1.0f)} * cell_size);

  //     auto& tree_transform = scene.get_component<sbx::scenes::transform>(tree);
  //     tree_transform.set_position(sbx::math::vector3{position.x(), 0.0f, position.y()});
  //     // tree_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});
  //     tree_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{sbx::math::random::next<std::float_t>(0.0f, 360.0f)});
  //   }
  // }

  // Rocks

  // auto rock_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  // rock_submeshes.push_back(sbx::scenes::static_mesh::submesh{0u, sbx::math::color::white(), sbx::scenes::static_mesh::material{}, scene.get_image("rocks")});

  // auto rock_1 = scene.create_node("Rock1");
  // scene.add_component<sbx::scenes::static_mesh>(rock_1, scene.get_mesh("rock_1"), rock_submeshes);
  // auto& rock_1_transform = scene.get_component<sbx::scenes::transform>(rock_1);
  // rock_1_transform.set_position(sbx::math::vector3{-6.0f, 0.0f, 0.0f});
  // rock_1_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // auto rock_2 = scene.create_node("Rock2");
  // scene.add_component<sbx::scenes::static_mesh>(rock_2, scene.get_mesh("rock_2"), rock_submeshes);
  // auto& rock_2_transform = scene.get_component<sbx::scenes::transform>(rock_2);
  // rock_2_transform.set_position(sbx::math::vector3{-3.0f, 0.0f, 0.0f});
  // rock_2_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // auto rock_3 = scene.create_node("Rock3");
  // scene.add_component<sbx::scenes::static_mesh>(rock_3, scene.get_mesh("rock_3"), rock_submeshes);
  // auto& rock_3_transform = scene.get_component<sbx::scenes::transform>(rock_3);
  // rock_3_transform.set_position(sbx::math::vector3{0.0f, 0.0f, 0.0f});
  // rock_3_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // auto rock_4 = scene.create_node("Rock4");
  // scene.add_component<sbx::scenes::static_mesh>(rock_4, scene.get_mesh("rock_4"), rock_submeshes);
  // auto& rock_4_transform = scene.get_component<sbx::scenes::transform>(rock_4);
  // rock_4_transform.set_position(sbx::math::vector3{3.0f, 0.0f, 0.0f});
  // rock_4_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // auto rock_5 = scene.create_node("Rock5");
  // scene.add_component<sbx::scenes::static_mesh>(rock_5, scene.get_mesh("rock_5"), rock_submeshes);
  // auto& rock_5_transform = scene.get_component<sbx::scenes::transform>(rock_5);
  // rock_5_transform.set_position(sbx::math::vector3{6.0f, 0.0f, 0.0f});
  // rock_5_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // // Cubes

  // auto height_map = std::vector<std::float_t>{};
  // height_map.resize(100 * 100);

  // for (auto y = 0; y < 100; ++y) {
  //   for (auto x = 0; x < 100; ++x) {
  //     height_map[x + y * 100] = sbx::math::noise::fractal(x * 0.05f, y * 0.05f, 5);
  //   }
  // }

  // scene.add_image("height_map", sbx::math::vector2u{100, 100}, VK_FORMAT_R32_SFLOAT, reinterpret_cast<const std::uint8_t*>(height_map.data()));

  // for (auto y = -3; y <= 3; y = y + 3) {
  //   for (auto x = -3; x <= 3; x = x + 3) {
  //     auto test = scene.create_node("Test");
  //     auto& test_transform = scene.get_component<sbx::scenes::transform>(test);
  //     test_transform.set_position(sbx::math::vector3{x, sbx::math::random::next<std::float_t>(6.0f, 8.0f), y});
  //     test_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});
  //     test_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{45});
  //     scene.add_component<rotator>(test);
  //     // scene.add_component<sbx::scenes::static_mesh>(test, _mesh_ids["sphere"], 0u, sbx::math::color{1.0f, 0.6f, 0.6f, 1.0f}, sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f});
  //     scene.add_component<sbx::scenes::static_mesh>(test, scene.get_mesh("cube"), 0u, sbx::math::color{0.39f, 0.44f, 0.56f, 1.0f}, sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f}, scene.get_image("height_map"));
  //   }
  // }

  // Camera
  auto camera = scene.camera();

  scene.add_component<sbx::scenes::skybox>(camera, scene.get_cube_image("skybox"));

  // const auto position = sbx::math::vector3{10.0f, 10.0f, 10.0f};

  // camera.get_component<sbx::scenes::transform>().set_position(position);
  // camera.get_component<sbx::scenes::transform>().look_at(sbx::math::vector3::zero);

  if (auto hide_window = cli.argument<bool>("hide-window"); !hide_window) {
    window.show();
  }

  sbx::utility::logger<"demo">::info("string id: {}", sbx::utility::string_id<"foobar">());

  // scenes_module.save_scene("res://scenes/test.yaml");
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
  const auto delta_time = sbx::core::engine::delta_time();

  if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
    sbx::core::engine::quit();
    return;
  }

  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  auto& scene = scenes_module.scene();

  if (sbx::devices::input::is_key_pressed(sbx::devices::key::space)) {
    const int grid_size = 5;
    const float spacing = 2.5f; // Adjust spacing based on cube size
    const sbx::math::vector3 base_position{0.0f, 20.0f, 5.0f};

    for (int x = 0; x < grid_size; ++x) {
      for (int z = 0; z < grid_size; ++z) {
        auto cube = scene.create_node("Cube");

        scene.add_component<sbx::scenes::static_mesh>(cube, scene.get_mesh("cube"), scene.get_material("grass3"));

        auto& transform = scene.get_component<sbx::scenes::transform>(cube);
        transform.set_position(base_position + sbx::math::vector3{x * spacing, 0.0f, z * spacing});
        transform.set_rotation(sbx::math::vector3{1.0, 0.0, 1.0}, sbx::math::degree{45});
        transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});

        auto& rigidbody = scene.add_component<sbx::physics::rigidbody>(cube, sbx::units::kilogram{1.0f});
        rigidbody.set_constant_acceleration({0.0f, -9.81f, 0.0f});

        const auto& collider = scene.add_component<sbx::physics::collider>(cube, sbx::physics::box{sbx::math::vector3{0.5f}});
        rigidbody.set_inverse_inertia_tensor_local(sbx::physics::local_inverse_inertia(rigidbody.mass(), collider));
      }
    }
  }

  static auto fox_speed = 0.0f;
  static auto direction = 1;

  fox_speed += direction * 0.2f * delta_time;

  if (fox_speed > 2.5f) {
    fox_speed = 2.5f;
    direction = -1;
  } else if (fox_speed < 0.0f) {
    fox_speed == 0.0f;
    direction = 1;
  }

  auto& fox_animator = scene.get_component<sbx::animations::animator>(fox1);
  fox_animator.set_float("speed", fox_speed);

  auto& fox_transform = scene.get_component<sbx::scenes::transform>(fox1);
  fox_transform.set_rotation(sbx::math::vector3::up, _rotation);

  _camera_controller.update();

  _rotation += sbx::math::degree{45} * delta_time;

  auto query_rotator = scene.query<sbx::scenes::transform, rotator>();

  for (auto&& [node, transform] : query_rotator.each()) {
    transform.set_rotation(sbx::math::vector3::up, _rotation);
  }

  auto query_walker = scene.query<sbx::scenes::transform, walker>();

  for (auto&& [node, transform] : query_walker.each()) {
    transform.move_by(transform.forward() * delta_time * 2.0f);
  }

  auto& light_center_transform = scene.get_component<sbx::scenes::transform>(_light_center);
  light_center_transform.set_rotation(sbx::math::vector3::up, _rotation);

  auto query_coordinates = scene.query<show_local_coordinates>();

  for (auto&& [node] : query_coordinates.each()) {
    const auto world = scene.world_transform(node);

    scenes_module.add_coordinate_arrows(world);
  }

  // for (auto& tank : _tanks) {
  //   tank.update();
  // }

  // const auto& image = static_cast<const sbx::graphics::image2d&>(graphics_module.attachment("object_id");
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
