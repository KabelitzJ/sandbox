#ifndef LIBSBX_SCENES_SCENE_MODULE_HPP_
#define LIBSBX_SCENES_SCENE_MODULE_HPP_

#include <memory>

#include <yaml-cpp/yaml.h>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/scenes/scene.hpp>

#include <libsbx/scenes/components/script.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/point_light.hpp>
#include <libsbx/scenes/components/directional_light.hpp>
#include <libsbx/scenes/components/camera.hpp>

namespace sbx::scenes {

class scenes_module final : public core::module<scenes_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::normal);

public:

  scenes_module()
  : _scene{nullptr} {
    register_loader("transform", [](node& node, const YAML::Node& node_data) {
      const auto position = node_data["position"].as<math::vector3>();
      const auto rotation = node_data["rotation"].as<math::vector3>();
      const auto scale = node_data["scale"].as<math::vector3>();

      node.add_component<math::transform>(position, rotation, scale);
    });

    register_loader("static_mesh", [](node& node, const YAML::Node& node_data) {
      const auto mesh_path = node_data["mesh"].as<std::string>();
      const auto texture_path = node_data["texture"].as<std::string>();

      auto& assets_manager = core::engine::get_module<assets::assets_module>();

      auto mesh_id = assets_manager.try_get_asset_id(std::filesystem::path{mesh_path});

      if (!mesh_id) {
        core::logger::warn("Mesh '{}' could not be found", mesh_path);
        return;
      }

      auto texture_id = assets_manager.try_get_asset_id(std::filesystem::path{texture_path});

      if (!texture_id) {
        core::logger::warn("Texture '{}' could not be found", texture_path);
        return;
      }

      node.add_component<scenes::static_mesh>(*mesh_id, *texture_id);
    });

    register_loader("camera", [this](node& node, const YAML::Node& node_data) {
      const auto fov = node_data["fov"].as<std::float_t>();
      const auto near = node_data["near"].as<std::float_t>();
      const auto far = node_data["far"].as<std::float_t>();

      auto& devices_module = core::engine::get_module<devices::devices_module>();

      auto& window = devices_module.window();

      node.add_component<camera>(math::degree{fov}, window.aspect_ratio(), near, far);
    });

    register_loader("script", [](node& node, const YAML::Node& node_data) {
      // [NOTE] KAJ 2023-10-29 : Remove any existing script component. We currently only support one script per entity
      node.remove_component<scenes::script>();

      const auto path = node_data["script"].as<std::string>();

      auto& script = node.add_component<scenes::script>(path);

      if (const auto parameters = node_data["parameters"]; parameters) {
        for (const auto& parameter : parameters) {
          const auto name = parameter["name"].as<std::string>();
          const auto type = parameter["type"].as<std::string>();

          if (type == "number") {
            const auto value = parameter["value"].as<std::float_t>();

            script.set(name, value);
          } else if (type == "vector3") {
            const auto value = parameter["value"].as<math::vector3>();

            script.set(name, value);
          } else if (type == "color") {
            const auto value = parameter["value"].as<math::color>();

            script.set(name, value);
          } else if (type == "string") {
            const auto value = parameter["value"].as<std::string>();

            script.set(name, value);
          } else {
            core::logger::warn("Unknown parameter type: {}", type);
          }
        }
      }
    });

    register_loader("point_light", [](node& node, const YAML::Node& node_data) {
      const auto color = node_data["color"].as<math::color>();
      const auto radius = node_data["radius"].as<std::float_t>();

      node.add_component<point_light>(color, radius);
    });

    register_loader("directional_light", [](node& node, const YAML::Node& node_data) {
      const auto direction = node_data["direction"].as<math::vector3>();
      const auto color = node_data["color"].as<math::color>();

      node.add_component<directional_light>(direction, color);
    });
  }

  ~scenes_module() override = default;

  auto update() -> void override {
    if (!_scene) {
      return;
    }

    auto script_nodes = _scene->query<scenes::script>();

    for (auto& node : script_nodes) {
      _update_script(node);
    }
  }

  auto load_scene(const std::filesystem::path& path) -> scenes::scene& {
    return *(_scene = std::make_unique<scenes::scene>(path));
  }

  auto scene() -> scenes::scene& {
    if (!_scene) {
      throw std::runtime_error{"No active scene"};
    }

    return *_scene;
  }

  template<std::invocable<node&, const YAML::Node&> Callable>
  auto register_loader(const std::string& name, Callable&& callable) -> void {
    _component_loaders.insert({name, std::forward<Callable>(callable)});
  }

private:

  auto _update_script(scenes::node& node) -> void {
    auto& transform = node.get_component<math::transform>();

    auto& script = node.get_component<scenes::script>();

    script.set("transform", transform);

    script.invoke("on_update");

    transform = script.get<math::transform>("transform");
  }

  std::unique_ptr<scenes::scene> _scene;
  std::unordered_map<std::string, std::function<void(node&, const YAML::Node&)>> _component_loaders;

}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
