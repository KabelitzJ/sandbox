#include <demo/demo_application.hpp>

#include <libsbx/math/color.hpp>

#include <demo/demo_renderer.hpp>

namespace demo {

demo_application::demo_application()
: sbx::core::application{},
  _rotation{sbx::math::degree{0}} {
  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  graphics_module.set_renderer<demo_renderer>();

  auto texture_ids = std::vector<sbx::math::uuid>{};

  const auto prototype_white_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/prototype_white.png");
  const auto base_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/base.png");
  const auto grid_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/grid.png");

  texture_ids.push_back(prototype_white_id);
  texture_ids.push_back(base_id);
  texture_ids.push_back(grid_id);

  const auto monkey_id = graphics_module.add_asset<sbx::models::mesh>("demo/assets/meshes/suzanne.obj");

  sbx::core::logger::debug("prototype_white_id: {}", prototype_white_id);
  sbx::core::logger::debug("monkey_id: {}", monkey_id);

  auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

  auto& window = devices_module.window();

  window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
    sbx::core::engine::quit();
  };

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.create_scene();

  for (auto i : std::views::iota(-2, 3)) {
    auto monkey = scene.create_node(fmt::format("Monkey{}", i));

    auto submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};

    submeshes.push_back(sbx::scenes::static_mesh::submesh{0, sbx::math::random_element(texture_ids)});

    monkey.add_component<sbx::scenes::static_mesh>(monkey_id, submeshes);

    monkey.get_component<sbx::math::transform>().set_position(sbx::math::vector3{static_cast<std::float_t>(i) * 3.0f, 0.0f, 0.0f});

    sbx::core::logger::debug("Created {} with id: {}", monkey.add_component<sbx::scenes::tag>(), sbx::math::uuid{monkey.get_component<sbx::scenes::id>()});
  }

  auto camera = scene.camera();

  camera.get_component<sbx::math::transform>().set_position(sbx::math::vector3{0.0f, 4.0f, 8.0f});
  camera.get_component<sbx::math::transform>().set_rotation(sbx::math::vector3::right, sbx::math::degree{26.565});

  auto& ui_module = sbx::core::engine::get_module<sbx::ui::ui_module>();

  auto& container = ui_module.container();

  static auto font = sbx::ui::font{"demo/assets/fonts/JetBrainsMono-Medium.ttf", sbx::ui::pixels{16}};

  auto label = container.add_widget<sbx::ui::label>("Hello, World!", sbx::math::vector2u{10, 10}, &font, 1.0f, sbx::math::color{1.0f, 0.0f, 0.0f, 1.0f});

  window.show();
}

auto demo_application::update() -> void  {
  if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
    sbx::core::engine::quit();
    return;
  }

  const auto dt = sbx::core::engine::delta_time();

  // _rotation += sbx::math::degree{45} * dt;

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  auto& scene = scenes_module.scene();

  for (auto& node : scene.query<sbx::scenes::static_mesh>()) {
    auto& transform = node.get_component<sbx::math::transform>();
    transform.set_rotation(sbx::math::vector3::up, _rotation);
  }
}

} // namespace demo
