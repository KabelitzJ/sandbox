#include <libsbx/scenes/scene.hpp>

#include <unordered_map>

#include <libsbx/units/time.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/math/angle.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/devices/devices_module.hpp>
#include <libsbx/devices/window.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/script.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/point_light.hpp>

namespace sbx::scenes {

scene::scene()
: _registry{}, 
  _root{&_registry, _registry.create_entity()},
  _camera{&_registry, _registry.create_entity()},
  _light{math::vector3{1.0, -1.0, 1.0}, math::color{1.0, 1.0, 1.0, 1.0}} {
  // [NOTE] KAJ 2023-10-17 : Initialize root node
  auto& root_id = _root.add_component<scenes::id>();
  _root.add_component<scenes::relationship>(root_id);
  _root.add_component<math::transform>();
  _root.add_component<scenes::tag>("ROOT");

  _nodes.insert({root_id, _root});

  // [NOTE] KAJ 2023-10-17 : Initialize camera node
  auto& camera_id = _camera.add_component<scenes::id>();

  _nodes.insert({camera_id, _camera});

  _camera.add_component<scenes::relationship>(root_id);
  _root.get_component<scenes::relationship>().add_child(camera_id);

  _camera.add_component<math::transform>();
  _camera.add_component<scenes::tag>("Camera");

  auto& devices_module = core::engine::get_module<devices::devices_module>();
  auto& window = devices_module.window();

  _camera.add_component<scenes::camera>(math::angle{math::radian{45.0f}}, window.aspect_ratio(), 0.1f, 100.0f, true);

  window.on_framebuffer_resized() += [this](const devices::framebuffer_resized_event& event) {
    auto& camera = _camera.get_component<scenes::camera>();
    camera.set_aspect_ratio(static_cast<std::float_t>(event.width) / static_cast<std::float_t>(event.height));
  };
}

scene::scene(const std::filesystem::path& path)
: scene{} {
  auto& assets_manager = core::engine::get_module<assets::assets_module>();

  const auto actual_path = assets_manager.asset_path(path);

  auto timer = utility::timer{};

  auto root_node = YAML::LoadFile(actual_path.string());

  const auto name = root_node["name"].as<std::string>();

  core::logger::debug("Scene name: {}", name);

  if (auto light_node = root_node["light"]; light_node) {
    auto direction = light_node["direction"].as<math::vector3>();
    auto color = light_node["color"].as<math::color>();

    _light = directional_light{direction, color};
  }

  if (auto wind_node = root_node["wind"]; wind_node) {
    _wind_speed = wind_node["speed"].as<std::float_t>();
  }

  _parse_assets(root_node);
  _parse_entities(root_node);

  core::logger::debug("Loaded scene: {} in {:.2f} ms", path.string(), units::quantity_cast<units::millisecond>(timer.elapsed()).value());
}

auto scene::start() -> void {
  auto script_nodes = query<scenes::script>();

  for (auto& node : script_nodes) {
    auto& script = node.get_component<scenes::script>();

    auto& transform = node.get_component<math::transform>();

    script.set("transform", transform);

    script.invoke("on_create");

    transform = script.get<math::transform>("transform");
  }
}

auto scene::create_child_node(node& parent, const std::string& tag, const math::transform& transform) -> node {
  auto node = scenes::node{&_registry, _registry.create_entity()};

  auto& id = node.add_component<scenes::id>();

  _nodes.insert({id, node});

  node.add_component<scenes::relationship>(parent.get_component<scenes::id>());
  parent.get_component<scenes::relationship>().add_child(id);

  node.add_component<math::transform>(transform);

  node.add_component<scenes::tag>(!tag.empty() ? tag : scenes::tag{"Node"});

  return node;
}

auto scene::create_node(const std::string& tag, const math::transform& transform) -> node {
  return create_child_node(_root, tag, transform);
}

auto scene::destroy_node(const node& node) -> void {
  const auto& id = node.get_component<scenes::id>();
  const auto& relationship = node.get_component<scenes::relationship>();

  for (auto& child : relationship.children()) {
    destroy_node(_nodes.at(child));
  }

  if (auto entry = _nodes.find(id); entry != _nodes.end()) {
    entry->second.get_component<scenes::relationship>().remove_child(id);
  } else {
    core::logger::warn("Node '{}' has invalid parent", node.get_component<scenes::tag>());
  }

  _nodes.erase(id);

  _registry.destroy_entity(node._entity);
}

auto scene::world_transform(const node& node) -> math::matrix4x4 {
  auto& transform = node.get_component<math::transform>();
  auto& relationship = node.get_component<scenes::relationship>();

  auto& parent = _nodes.at(relationship.parent());

  auto world = math::matrix4x4::identity;

  if (parent.get_component<scenes::id>() != _root.get_component<scenes::id>()) {
    world = world_transform(parent);
  }

  return world * transform.as_matrix();
}

auto scene::_parse_assets(const YAML::Node& root_node) -> void {
  auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

  const auto assets_node = root_node["assets"];

  auto& asset_loaders = scenes_module._asset_loaders;

  if (auto entry = asset_loaders.find("texture"); entry != asset_loaders.end()) {
    const auto textures_node = assets_node["textures"].as<std::vector<YAML::Node>>();

    for (const auto& texture_node : textures_node) {
      std::invoke(entry->second, texture_node);
    }
  } else {
    core::logger::warn("Unknown component type: texture");
  }

  if (auto entry = asset_loaders.find("mesh"); entry != asset_loaders.end()) {
    const auto meshes_node = assets_node["meshes"].as<std::vector<YAML::Node>>();

    for (const auto& mesh_node : meshes_node) {
      std::invoke(entry->second, mesh_node);
    }
  } else {
    core::logger::warn("Unknown component type: mesh");
  }
}

auto scene::_parse_entities(const YAML::Node& root_node) -> void {
  auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
  
  const auto entities = root_node["entities"].as<std::vector<YAML::Node>>();

  auto& component_loaders = scenes_module._component_loaders;

  for (const auto& entity_node : entities) {
    const auto entity_name = entity_node["name"].as<std::string>();

    core::logger::debug("  Entity name: {}", entity_name);

    auto entity = create_node(entity_name);

    const auto components = entity_node["components"].as<std::vector<YAML::Node>>();

    for (const auto& component_node : components) {
      const auto component_type = component_node["type"].as<std::string>();

      if (auto entry = component_loaders.find(component_type); entry != component_loaders.end()) {
        core::logger::debug("    Component type: {}", component_type);

        // entry->second(entity, component_node);
        std::invoke(entry->second, entity, component_node);

        if (component_type == "camera") {
          _camera = entity;
        }
      } else {
        core::logger::warn("Unknown component type: {}", component_type);
      }
    }
  }
}

} // namespace sbx::scenes
