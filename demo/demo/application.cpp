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

  // auto& compiler = graphics_module.compiler();

  // const auto request = sbx::graphics::compiler::compile_request{
  //   .path = "demo/assets/shaders/deferred_static_material",
  //   .per_stage = {
  //     {SLANG_STAGE_VERTEX, {
  //       .entry_point = "main"
  //     }},
  //     {SLANG_STAGE_FRAGMENT, {
  //       .entry_point = "mask_main"
  //     }},
  //   }
  // };

  // const auto result = compiler.compile(request);

  // for (const auto& [stage, words] : result.code) {
  //   const auto file_name = stage == SLANG_STAGE_FRAGMENT ? "frag.spv" : "vert.spv";

  //   std::ofstream file(file_name, std::ios::binary);

  //   if (!file) {
  //     std::cerr << "Failed to open " << file_name << " for writing\n";
  //     continue;
  //   }

  //   file.write(reinterpret_cast<const char*>(words.data()), words.size() * sizeof(uint32_t));
  //   file.close();
  // }

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.load_scene("res://scenes/scene.yaml");

  auto& scripting_module = sbx::core::engine::get_module<sbx::scripting::scripting_module>();

  scripting_module.test();

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
  scene.add_image("helmet_mrao", "res://textures/helmet/mrao2.jpg");
  scene.add_image("helmet_emissive", "res://textures/helmet/emissive.jpg");

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

  scene.add_image("longhouse_albedo", "res://textures/longhouse/albedo.jpg");
  scene.add_image("longhouse_normal", "res://textures/longhouse/normal.jpg");
  scene.add_image("longhouse_mrao", "res://textures/longhouse/mrao.jpg");

  scene.add_image("pine_tree_bark_albedo", "res://textures/pine_tree/bark_albedo.png");
  scene.add_image("pine_tree_bark_normal", "res://textures/pine_tree/bark_normal.png");
  scene.add_image("pine_tree_leaves_albedo", "res://textures/pine_tree/leaves_albedo.png");
  scene.add_image("pine_tree_leaves_normal", "res://textures/pine_tree/leaves_normal.png");

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

  scene.add_mesh<sbx::models::mesh>("longhouse", "res://meshes/longhouse/longhouse.gltf");

  scene.add_mesh<sbx::models::mesh>("pine_tree", "res://meshes/pine_tree/pine_tree.gltf");

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

  // // Longhouse

  // auto longhouse = scene.create_node("Longhouse");

  // auto& longhouse_material = scene.add_material<sbx::models::material>("longhouse");
  // longhouse_material.albedo = scene.get_image("longhouse_albedo");
  // longhouse_material.normal = scene.get_image("longhouse_normal");
  // longhouse_material.mrao = scene.get_image("longhouse_mrao");

  // scene.add_component<sbx::scenes::static_mesh>(longhouse, scene.get_mesh("longhouse"), scene.get_material("longhouse"));

  // auto& longhouse_transform = scene.get_component<sbx::scenes::transform>(longhouse);
  // longhouse_transform.set_position(sbx::math::vector3{-15.0f, 0.0f, -10.0f});
  // longhouse_transform.set_scale(sbx::math::vector3{2.5f, 2.5f, 2.5f});

  // // Pine Tree

  // auto pine_tree = scene.create_node("PineTree");

  // auto& pine_tree_bark = scene.add_material<sbx::models::material>("pine_tree_bark");
  // pine_tree_bark.albedo = scene.get_image("pine_tree_bark_albedo");
  // pine_tree_bark.normal = scene.get_image("pine_tree_bark_normal");
  
  // auto& pine_tree_leaves= scene.add_material<sbx::models::material>("pine_tree_leaves");
  // pine_tree_leaves.roughness = 0.2f;
  // pine_tree_leaves.albedo = scene.get_image("pine_tree_leaves_albedo");
  // pine_tree_leaves.normal = scene.get_image("pine_tree_leaves_normal");
  // pine_tree_leaves.alpha = sbx::models::alpha_mode::mask;
  // pine_tree_leaves.is_double_sided = true;

  // auto pine_tree_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{
  //   {0u, scene.get_material("pine_tree_bark")},
  //   {1u, scene.get_material("pine_tree_leaves")}
  // };

  // scene.add_component<sbx::scenes::static_mesh>(pine_tree, scene.get_mesh("pine_tree"), pine_tree_submeshes);

  // auto& pine_tree_transform = scene.get_component<sbx::scenes::transform>(pine_tree);
  // pine_tree_transform.set_position(sbx::math::vector3{-10.0f, 0.0f, -5.0f});
  // pine_tree_transform.set_scale(sbx::math::vector3{0.5f, 0.5f, 0.5f});

  // Circling point lights
  _light_center = scene.create_node("LightCenter", sbx::scenes::transform{sbx::math::vector3{0.0f, 20.0f, 0.0f}});
  
  const auto radius = 20.0f;
  const auto light_count = 8;
  
  for (auto i = 0; i < light_count; ++i) {
    auto angle = sbx::math::radian{2.0f * sbx::math::pi / static_cast<std::float_t>(light_count) * static_cast<std::float_t>(i)};

    const auto material_name = fmt::format("Light{}", i);
    const auto color = sbx::math::random_color(0.5f);

    auto& material = scene.add_material<sbx::models::material>(material_name);
    material.base_color = color;
    material.alpha = sbx::models::alpha_mode::blend;

    auto light = scene.create_child_node(_light_center, fmt::format("Light{}", i), sbx::scenes::transform{sbx::math::vector3{radius * sbx::math::cos(angle), 0.0f, radius * sbx::math::sin(angle)}});

    scene.add_component<sbx::scenes::point_light>(light, color, 50.0f);

    scene.add_component<sbx::scenes::static_mesh>(light, scene.get_mesh("sphere"), scene.get_material(material_name));

    auto& light_transform = scene.get_component<sbx::scenes::transform>(light);
    // light_transform.set_scale(sbx::math::vector3{0.2f, 0.2f, 0.2f});
  }

  // Helmet
  auto helmet = scene.create_node("Helmet");

  // auto helmet_material = scene.add_material<sbx::models::material>("helmet", sbx::scenes::material_type::opaque, sbx::math::color::white(), 0.0f, 0.5f, 1.0f, scene.get_image("helmet_albedo"), scene.get_image("helmet_normal"), scene.get_image("helmet_mrao"));
  auto& helmet_material = scene.add_material<sbx::models::material>("helmet");
  helmet_material.albedo = scene.get_image("helmet_albedo");
  helmet_material.normal = scene.get_image("helmet_normal");
  helmet_material.mrao = scene.get_image("helmet_mrao");
  helmet_material.emissive = scene.get_image("helmet_emissive");
  helmet_material.emissive_factor = sbx::math::vector4{1, 1, 1, 0};

  scene.add_component<sbx::scenes::static_mesh>(helmet, scene.get_mesh("helmet"), scene.get_material("helmet"));

  auto& helmet_transform = scene.get_component<sbx::scenes::transform>(helmet);
  helmet_transform.set_position(sbx::math::vector3{0.0f, 6.0f, 0.0f});
  helmet_transform.set_scale(sbx::math::vector3{4.0f, 4.0f, 4.0f});

  // scripting_module.instantiate(helmet, "res://scripts/test.lua");

  // Dragon
  // auto& dragon_mesh = assets_module.get_asset<sbx::models::mesh>(scene.get_mesh("dragon"));

  auto dragon = scene.create_node("Dragon");

  // scene.add_material<sbx::scenes::material>("cloth", sbx::scenes::material_type::opaque, sbx::math::color::blue(), 0.0f, 1.0f, 1.0f, scene.get_image("checkerboard"));
  // scene.add_material<sbx::scenes::material>("dragon", sbx::scenes::material_type::transparent, sbx::math::color{0.0f, 0.6588f, 0.4196f, 0.6f}, 0.0f, 0.5f, 1.0f);

  auto& dragon_material = scene.add_material<sbx::models::material>("dragon");
  dragon_material.base_color = sbx::math::color{0.0f, 0.6588f, 0.4196f, 0.6f};
  dragon_material.alpha = sbx::models::alpha_mode::blend;
  // dragon_material.alpha = sbx::models::alpha_mode::opaque;
  // dragon_material.is_double_sided = true;

  auto& cloth_material = scene.add_material<sbx::models::material>("cloth");
  cloth_material.base_color = sbx::math::color::green();
  cloth_material.albedo = scene.get_image("checkerboard");

  scene.add_component<sbx::scenes::static_mesh>(dragon, scene.get_mesh("dragon"), std::vector<sbx::scenes::static_mesh::submesh>{{0u, scene.get_material("cloth")}, {1u, scene.get_material("dragon")}});

  auto& dragon_transform = scene.get_component<sbx::scenes::transform>(dragon);
  dragon_transform.set_position(sbx::math::vector3{-8.0f, 2.0f, 4.0f});
  dragon_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{45});
  dragon_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // Fox
  auto& animations_module = sbx::core::engine::get_module<sbx::animations::animations_module>();

  fox1 = scene.create_node("Fox");

  // scripting_module.instantiate(fox1, "res://scripts/test.lua");

  auto& fox_material = scene.add_material<sbx::models::material>("fox");
  fox_material.albedo = scene.get_image("fox_albedo");
  fox_material.roughness = 0.7f;
  fox_material.occlusion = 0.8f;

  animations_module.add_animation(fox1, scene.get_mesh("fox"), scene.get_animation("Walk"), scene.get_material("fox"));

  auto fox_tail_node = animations_module.find_skeleton_node(fox1, "b_Tail03_014");

  if (fox_tail_node != sbx::scenes::node::null) {
    auto test = scene.create_child_node(fox_tail_node, "Test");

    scene.add_component<sbx::scenes::static_mesh>(test, scene.get_mesh("sphere"), scene.get_material("fox"));

    auto& test_transform = scene.get_component<sbx::scenes::transform>(test);
    test_transform.set_scale(sbx::math::vector3{10.0f, 10.0f, 10.0f});
  }

  auto& fox_animator = scene.add_component<sbx::animations::animator>(fox1);

  fox_animator.add_state({"Walk", scene.get_animation("Walk"), true, 0.5f });
  fox_animator.add_state({"Survey", scene.get_animation("Survey"), true, 0.5f });
  fox_animator.add_state({"Run", scene.get_animation("Run"), true, 0.5f });

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
  fox1_transform.set_position(sbx::math::vector3{12.0f, 0.0f, 0.0f});
  fox1_transform.set_scale(sbx::math::vector3{0.06f, 0.06f, 0.06f});

  auto spheres = scene.create_node(fmt::format("Spheres"));

  auto& spheres_transform = scene.get_component<sbx::scenes::transform>(spheres);
  spheres_transform.set_position(sbx::math::vector3{0, 0, -15});

  for (auto y = 0; y < 5; ++y) {
    for (auto x = 0; x < 5; ++x) {
      auto sphere = scene.create_child_node(spheres, fmt::format("Sphere{}{}", x, y));

      const auto material_name = fmt::format("sphere_{}_{}_material", x, y);

      auto& material = scene.add_material<sbx::models::material>(material_name);
      material.base_color = sbx::math::color::white();
      material.alpha = sbx::models::alpha_mode::opaque;
      material.metallic = 0.2f * x;
      material.roughness = 0.2f * y;
      material.occlusion = 1.0f;
      material.albedo = scene.get_image("checkerboard");

      scene.add_component<sbx::scenes::static_mesh>(sphere, scene.get_mesh("sphere"), scene.get_material(material_name));

      auto& sphere_transform = scene.get_component<sbx::scenes::transform>(sphere);
      sphere_transform.set_position(sbx::math::vector3{x * 3, y * 3 + 5, 0.0f});
      sphere_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});
    }
  }

  // Camera
  auto camera = scene.camera();

  scene.add_component<sbx::scenes::skybox>(camera, scene.get_cube_image("skybox"));

  if (auto hide_window = cli.argument<bool>("hide-window"); !hide_window) {
    window.show();
  }

  sbx::utility::logger<"demo">::info("string id: {}", sbx::utility::string_id<"foobar">());
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

  _rotation += sbx::math::degree{45} * delta_time;

  _camera_controller.update();

  // static auto fox_speed = 0.0f;
  // static auto direction = 1;

  // fox_speed += direction * 0.2f * delta_time;

  // if (fox_speed > 2.5f) {
  //   fox_speed = 2.5f;
  //   direction = -1;
  // } else if (fox_speed < 0.0f) {
  //   fox_speed = 0.0f;
  //   direction = 1;
  // }

  // auto& fox_animator = scene.get_component<sbx::animations::animator>(fox1);
  // fox_animator.set_float("speed", fox_speed);

  // auto& fox_transform = scene.get_component<sbx::scenes::transform>(fox1);
  // fox_transform.set_rotation(sbx::math::vector3::up, _rotation);

  if (scene.is_valid(_light_center)) {
    auto& light_center_transform = scene.get_component<sbx::scenes::transform>(_light_center);
    light_center_transform.set_rotation(sbx::math::vector3::up, _rotation);
  }
}

auto application::fixed_update() -> void {

}

} // namespace demo
