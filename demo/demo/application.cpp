#include <demo/application.hpp>

#include <nlohmann/json.hpp>

#include <libsbx/math/color.hpp>

#include <demo/renderer.hpp>
#include <demo/line.hpp>

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

  // const auto prototype_white_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/prototype_white.png");
  // const auto prototype_black_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/prototype_black.png");
  // const auto base_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/base.png");
  // const auto grid_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/grid.png");
  // const auto wood_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/wood.png"); 
  // const auto white_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/white.png");
  // const auto checkerboard_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/checkerboard.jpg");
  // const auto hex_grid_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/hex_grid.png");

  // _texture_ids.emplace("prototype_white", prototype_white_id);
  // _texture_ids.emplace("prototype_black", prototype_black_id);  
  // _texture_ids.emplace("base", base_id);
  // _texture_ids.emplace("grid", grid_id);
  // _texture_ids.emplace("wood", wood_id);
  // _texture_ids.emplace("white", white_id);
  // _texture_ids.emplace("checkerboard", checkerboard_id);
  // _texture_ids.emplace("hex_grid", hex_grid_id);

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

  // const auto monkey_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/suzanne/suzanne.gltf");
  // const auto sphere_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/sphere/sphere.gltf");
  // const auto crate_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/crate/crate.gltf");
  // const auto cube_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/cube/cube.gltf");
  // const auto tree_2_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/tree_2/tree_2.gltf");
  // const auto tree_1_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/tree_1/tree_1.gltf");
  // const auto dragon_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/dragon/dragon.gltf");

  // const auto line_mesh_id = graphics_module.add_asset<line_mesh>(generate_grid(sbx::math::vector2u{11u, 11u}, sbx::math::vector2{1.0f, 1.0f}));

  // _mesh_ids.emplace("monkey", monkey_id);
  // _mesh_ids.emplace("plane", plane_id);
  // _mesh_ids.emplace("sphere", sphere_id);
  // _mesh_ids.emplace("crate", crate_id);
  // _mesh_ids.emplace("cube", cube_id);
  // _mesh_ids.emplace("tree_2", tree_2_id);
  // _mesh_ids.emplace("tree_1", tree_1_id);
  // _mesh_ids.emplace("dragon", dragon_id);
  // _mesh_ids.emplace("line_mesh", line_mesh_id);

  // Window

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  // Scene

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.create_scene();

  // Plane

  auto plane = scene.create_node("Plane");

  auto plane_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  plane_submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::color::white, _texture_ids[texture_map["plane_texture"].get<std::string>()]});

  plane.add_component<sbx::scenes::static_mesh>(_mesh_ids["plane"], plane_submeshes);

  auto& plane_transform = plane.get_component<sbx::math::transform>();
  plane_transform.set_scale(sbx::math::vector3{1.0f, 1.0f, 1.0f});

  plane.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  plane.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-50.0f, 0.0f, -50.0f}, sbx::math::vector3{50.0f, 0.0f, 50.0f}});

  _plane_id = plane.get_component<sbx::scenes::id>();

  // // Light 0

  // auto light0 = scene.create_node("Light0");

  // _light_id = light0.get_component<sbx::scenes::id>();

  // // Ligth 1

  // auto light1 = scene.create_child_node(light0, "Light1");

  // light1.add_component<sbx::scenes::gizmo>(_mesh_ids["sphere"], 0u, _texture_ids["white"], sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  // auto& light1_transform = light1.get_component<sbx::math::transform>();

  // light1_transform.set_position(sbx::math::vector3{5.0f, 5.0f, 5.0f});
  // // light1_transform.set_scale(sbx::math::vector3{0.5f, 0.5f, 0.5f});

  // light1.add_component<sbx::scenes::point_light>(sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f}, 20.0f);

  // // Ligth 2

  // auto light2 = scene.create_child_node(light0, "Light2");

  // light2.add_component<sbx::scenes::gizmo>(_mesh_ids["sphere"], 0u, _texture_ids["white"], sbx::math::color{0.0f, 1.0f, 0.0f, 1.0f});

  // auto& light2_transform = light2.get_component<sbx::math::transform>();

  // light2_transform.set_position(sbx::math::vector3{-5.0f, 5.0f, -5.0f});
  // // light2_transform.set_scale(sbx::math::vector3{0.5f, 0.5f, 0.5f});

  // light2.add_component<sbx::scenes::point_light>(sbx::math::color{0.0f, 1.0f, 0.0f, 1.0f}, 20.0f);

  // // Ligth 3

  // auto light3 = scene.create_child_node(light0, "Light3");

  // light3.add_component<sbx::scenes::gizmo>(_mesh_ids["sphere"], 0u, _texture_ids["white"], sbx::math::color{0.0f, 0.0f, 1.0f, 1.0f});

  // auto& light3_transform = light3.get_component<sbx::math::transform>();

  // light3_transform.set_position(sbx::math::vector3{5.0f, 5.0f, -5.0f});
  // // light3_transform.set_scale(sbx::math::vector3{0.5f, 0.5f, 0.5f});

  // light3.add_component<sbx::scenes::point_light>(sbx::math::color{0.0f, 0.0f, 1.0f, 1.0f}, 20.0f);

  // // Light 4

  // auto light4 = scene.create_child_node(light0, "Light4");

  // light4.add_component<sbx::scenes::gizmo>(_mesh_ids["sphere"], 0u, _texture_ids["white"], sbx::math::color{1.0f, 1.0f, 0.0f, 1.0f});

  // auto& light4_transform = light4.get_component<sbx::math::transform>();

  // light4_transform.set_position(sbx::math::vector3{-5.0f, 5.0f, 5.0f});
  // // light4_transform.set_scale(sbx::math::vector3{0.5f, 0.5f, 0.5f});

  // light4.add_component<sbx::scenes::point_light>(sbx::math::color{1.0f, 1.0f, 0.0f, 1.0f}, 20.0f);

  // // Test1

  // auto test1 = scene.create_node("Test1");

  // test1.add_component<sbx::scenes::static_mesh>(_mesh_ids["cube"], _texture_ids["white"], sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  // auto& test1_transform = test1.get_component<sbx::math::transform>();
  // test1_transform.set_position(sbx::math::vector3{-30.0f, 1.0f, -25.0f});
  // test1_transform.set_scale(sbx::math::vector3{1.0f, 2.0f, 20.0f});

  // test1.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  // test1.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-0.5f, -1.0f, -10.0f}, sbx::math::vector3{0.5f, 1.0f, 10.0f}});

  // // Test2

  // auto test2 = scene.create_node("Test2");

  // test2.add_component<sbx::scenes::static_mesh>(_mesh_ids["cube"], _texture_ids["white"], sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  // auto& test2_transform = test2.get_component<sbx::math::transform>();
  // test2_transform.set_position(sbx::math::vector3{-10.0f, 1.0f, -25.0f});
  // test2_transform.set_scale(sbx::math::vector3{1.0f, 2.0f, 20.0f});

  // test2.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  // test2.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-0.5f, -1.0f, -10.0f}, sbx::math::vector3{0.5f, 1.0f, 10.0f}});

  // // Test3

  // auto test3 = scene.create_node("Test3");

  // test3.add_component<sbx::scenes::static_mesh>(_mesh_ids["cube"], _texture_ids["white"], sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  // auto& test3_transform = test3.get_component<sbx::math::transform>();
  // test3_transform.set_position(sbx::math::vector3{-20.0f, 1.0f, -15.0f});
  // test3_transform.set_scale(sbx::math::vector3{20.0f, 2.0f, 1.0f});

  // test3.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  // test3.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-10.0f, -1.0f, -0.5f}, sbx::math::vector3{10.0f, 1.0f, 0.5f}});

  // // Test4

  // auto test4 = scene.create_node("Test4");

  // test4.add_component<sbx::scenes::static_mesh>(_mesh_ids["cube"], _texture_ids["white"], sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  // auto& test4_transform = test4.get_component<sbx::math::transform>();
  // test4_transform.set_position(sbx::math::vector3{-20.0f, 1.0f, -35.0f});
  // test4_transform.set_scale(sbx::math::vector3{20.0f, 2.0f, 1.0f});

  // test4.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  // test4.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-10.0f, -1.0f, -0.5f}, sbx::math::vector3{10.0f, 1.0f, 0.5f}});

  // // Sphere1

  // auto sphere1 = scene.create_node("Sphere");

  // sphere1.add_component<sbx::scenes::static_mesh>(_mesh_ids["sphere"], _texture_ids["prototype_black"]);
  
  // auto& sphere1_transform = sphere1.get_component<sbx::math::transform>();
  // sphere1_transform.set_position(sbx::math::vector3{5.0f, 10.0f, 5.0f});
  
  // auto& sphere1_rigidbody = sphere1.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{1.0f});
  // sphere1_rigidbody.set_acceleration(sbx::math::vector3{0.0f, -9.81f, 0.0f});

  // sphere1.add_component<sbx::physics::collider>(sbx::physics::sphere{1.0f});

  // // Sphere2

  // auto sphere2 = scene.create_node("Sphere");

  // sphere2.add_component<sbx::scenes::static_mesh>(_mesh_ids["sphere"], _texture_ids["prototype_black"]);
  
  // auto& sphere2_transform = sphere2.get_component<sbx::math::transform>();
  // sphere2_transform.set_position(sbx::math::vector3{5.0f, 15.0f, 5.0f});
  
  // auto& sphere2_rigidbody = sphere2.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{1.0f});
  // sphere2_rigidbody.set_acceleration(sbx::math::vector3{0.0f, -9.81f, 0.0f});

  // sphere2.add_component<sbx::physics::collider>(sbx::physics::sphere{1.0f});

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
  
  auto& dragon_transform = dragon.get_component<sbx::math::transform>();
  // dragon_transform.set_position(sbx::math::vector3{-7.0f, 1.0f, -7.0f});
  // dragon_transform.set_position(sbx::math::vector3{0.0f, 3.0f, 0.0f});
  // dragon_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});
  // dragon_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{45});

  // // Crate

  // auto crate = scene.create_node("Crate");

  // crate.add_component<sbx::scenes::static_mesh>(_mesh_ids["crate"], _texture_ids["wood"]);
  
  // auto& crate_transform = crate.get_component<sbx::math::transform>();
  // crate_transform.set_position(sbx::math::vector3{-4.0f, 1.0f, 3.5f});
  // crate_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});
  // crate_transform.set_rotation(sbx::math::vector3::up, sbx::math::degree{20});

  // crate.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{1.0f}, true);

  // crate.add_component<sbx::physics::collider>(sbx::physics::box{sbx::math::vector3{-1.0f, -1.0f, -1.0f}, sbx::math::vector3{1.0f, 1.0f, 1.0f}});

  // // Tree 1

  // auto submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
  // submeshes.push_back(sbx::scenes::static_mesh::submesh{0, _texture_ids["white"], sbx::math::color{0.38f, 0.54f, 0.24f, 1.0f}});
  // submeshes.push_back(sbx::scenes::static_mesh::submesh{1, _texture_ids["white"], sbx::math::color{0.47f, 0.37f, 0.24f, 1.0f}});

  // auto tree1 = scene.create_node("Tree1");

  // tree1.add_component<sbx::scenes::static_mesh>(_mesh_ids["tree_1"], submeshes);
  
  // auto& tree1_transform = tree1.get_component<sbx::math::transform>();
  // tree1_transform.set_position(sbx::math::vector3{0.0f, 0.0f, -4.0f});
  // tree1_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // // Tree 2

  // auto tree2 = scene.create_node("Tree2");

  // tree2.add_component<sbx::scenes::static_mesh>(_mesh_ids["tree_2"], _texture_ids["white"], sbx::math::color{0.38f, 0.54f, 0.24f, 1.0f});
  
  // auto& tree2_transform = tree2.get_component<sbx::math::transform>();
  // tree2_transform.set_position(sbx::math::vector3{8.0f, 0.0f, -4.0f});
  // tree2_transform.set_scale(sbx::math::vector3{2.0f, 2.0f, 2.0f});

  // // Shield

  // auto shield = scene.create_node("Shield");

  // shield.add_component<sbx::scenes::gizmo>(_mesh_ids["sphere"], 0u, _texture_ids["hex_grid"], sbx::math::color{0.47f, 0.71f, 0.78f, 1.0f});

  // auto& shield_transform = shield.get_component<sbx::math::transform>();
  // shield_transform.set_position(sbx::math::vector3{10.0f, 0.0f, 10.0f});
  // shield_transform.set_scale(sbx::math::vector3{8.0f, 8.0f, 8.0f});

  // shield.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{0.0f}, true);

  // shield.add_component<sbx::physics::collider>(sbx::physics::sphere{8.0f});

  // // Monkeys

  // for (const auto [i, texture_id_entry] : ranges::views::enumerate(_texture_ids)) {
  //   auto monkey = scene.create_node(fmt::format("Monkey{}", i));

  //   const auto& id = monkey.get_component<sbx::scenes::id>();

  //   monkey.add_component<sbx::scenes::static_mesh>(_mesh_ids["monkey"], texture_id_entry.second);
    
  //   const auto spacing = 3.0f;

  //   const auto total_length = static_cast<std::float_t>(_texture_ids.size() - 1) * spacing;

  //   const auto min = -total_length / 2.0f;

  //   const auto x = min + static_cast<std::float_t>(i) * spacing;

  //   auto& monkey_transform = monkey.get_component<sbx::math::transform>();
  //   monkey_transform.set_position(sbx::math::vector3{x, 2.0f, 0.0f});

  //   monkey.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{1.0f}, true);
  //   monkey.add_component<sbx::physics::collider>(sbx::physics::sphere{1.0f});

  //   _monkey_ids.push_back(id);
  // }

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

  // auto light0 = scene.find_node(_light_id);

  // auto& light0_transform = light0->get_component<sbx::math::transform>();

  // light0_transform.set_rotation(sbx::math::vector3::up, _rotation);

  // for (const auto monkey_id : _monkey_ids) {
  //   auto monkey = scene.find_node(monkey_id);

  //   auto& transform = monkey->get_component<sbx::math::transform>();

  //   transform.set_rotation(sbx::math::vector3::up, _rotation);
  // }

  // auto& camera_transform = camera_node.get_component<sbx::math::transform>();

  // if (sbx::devices::input::is_key_pressed(sbx::devices::key::space)) {
  //   auto sphere = scene.create_node("Sphere");

  //   sphere.add_component<sbx::scenes::static_mesh>(_mesh_ids["sphere"], _texture_ids["white"], sbx::math::random_color());

  //   const auto& right = camera_transform.right();

  //   // const auto forward = sbx::math::vector3::normalized(sbx::math::vector3::cross(sbx::math::vector3::up, right));
  //   const auto& forward = camera_transform.forward();

  //   auto& sphere_transform = sphere.get_component<sbx::math::transform>();
  //   sphere_transform.set_position(camera_transform.position() + forward * 2.0f);
  //   sphere_transform.set_scale(sbx::math::vector3{0.5f, 0.5f, 0.5f});

  //   auto& sphere_rigidbody = sphere.add_component<sbx::physics::rigidbody>(sbx::units::kilogram{1.0f});
  //   sphere_rigidbody.set_acceleration(sbx::math::vector3{0.0f, -9.81f, 0.0f});

  //   sphere_rigidbody.set_velocity(forward * 20.0f);

  //   sphere.add_component<sbx::physics::collider>(sbx::physics::sphere{0.5f});

  //   sphere.add_component<ball_tag>();
  // }

  // if (sbx::devices::input::is_key_pressed(sbx::devices::key::r)) {
  //   auto balls = scene.query<ball_tag>();

  //   for (const auto& ball : balls) {
  //     scene.destroy_node(ball);
  //   }
  // }

  // _rotation += sbx::math::degree{45} * delta_time;

  // for (const auto& monkey_id : _monkey_ids) {
  //   auto monkey = scene.find_node(monkey_id);

  //   auto& transform = monkey->get_component<sbx::math::transform>();

  //   transform.set_rotation(sbx::math::vector3::up, _rotation);
  // }

  // auto to_destroy = std::vector<sbx::scenes::node>{};

  // for (const auto& node : scene.query<sbx::math::transform>()) {
  //   auto& transform = node.get_component<sbx::math::transform>();

  //   if (transform.position().y() < -5.0f) {
  //     to_destroy.push_back(node);
  //   }
  // }

  // for (const auto& node : to_destroy) {
  //   scene.destroy_node(node);
  // }
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
