#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scene.hpp>

#include <libsbx/scenes/components/transform.hpp>
#include <libsbx/scenes/components/skybox.hpp>

namespace sbx::scenes {

scenes_module::scenes_module()
: _scene{std::nullopt} {
  _component_io_registry.register_component<scenes::transform>(
    "transform",
    [](YAML::Node& node, scenes::scene& scene, const scenes::transform& transform) -> void {
      node["position"] = transform.position();
      node["rotation"] = transform.rotation();
      node["scale"] = transform.scale();
    },
    [](const YAML::Node& node) -> scenes::transform {
      return {node["position"].as<math::vector3>(), node["rotation"].as<math::quaternion>(), node["scale"].as<math::vector3>()};
    }
  );

  // _component_io_registry.register_component<scenes::skybox>(
  //   "skybox",
  //   [](YAML::Node& node, scenes::scene& scene, const scenes::skybox& skybox) -> void {
  //     node["image"] = YAML::Alias("");
  //     node["tint"] = skybox.tint;
  //   },
  //   [](const YAML::Node& node) -> scenes::skybox {
  //     return {node["image"].as<math::quaternion>(), node["tint"].as<math::color>()};
  //   }
  // );
}

scenes_module::~scenes_module() {

}

auto scenes_module::update() -> void {

}

auto scenes_module::load_scene(const std::filesystem::path& path) -> scenes::scene& {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  _scene.emplace(assets_module.resolve_path(path));

  return *_scene;
}

auto scenes_module::scene() -> scenes::scene& {
  return *_scene;
}

} // namespace sbx::scenes
