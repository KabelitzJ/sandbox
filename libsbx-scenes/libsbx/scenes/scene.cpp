#include <libsbx/scenes/scene.hpp>

#include <libsbx/units/time.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/math/angle.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/devices/devices_module.hpp>
#include <libsbx/devices/window.hpp>

#include <libsbx/assets/assets_module.hpp>

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
  _camera{&_registry, _registry.create_entity()} {
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
  auto& devices_module = core::engine::get_module<devices::devices_module>();

  const auto actual_path = assets_manager.asset_path(path);

  auto timer = utility::timer{};

  auto root_node = YAML::LoadFile(actual_path.string());

  const auto name = root_node["name"].as<std::string>();

  core::logger::debug("Scene name: {}", name);

  const auto entities = root_node["entities"].as<std::vector<YAML::Node>>();

  for (const auto& entity_node : entities) {
    const auto entity_name = entity_node["name"].as<std::string>();

    core::logger::debug("  Entity name: {}", entity_name);

    auto entity = create_node(entity_name);

    const auto components = entity_node["components"].as<std::vector<YAML::Node>>();

    for (const auto& component_node : components) {
      const auto component_type = component_node["type"].as<std::string>();

      core::logger::debug("    Component type: {}", component_type);

      if (component_type == "Transform") {
        const auto position = component_node["position"].as<math::vector3>();
        const auto euler_angles = component_node["rotation"].as<math::vector3>();
        const auto scale = component_node["scale"].as<math::vector3>();

        _add_or_update_component<math::transform>(entity, position, euler_angles, scale);
      } else if (component_type == "StaticMesh") {
        const auto mesh_path = component_node["mesh"].as<std::string>();
        const auto texture_path = component_node["texture"].as<std::string>();

        auto mesh_id = assets_manager.try_get_asset_id(std::filesystem::path{mesh_path});

        if (!mesh_id) {
          core::logger::warn("Mesh '{}' could not be found", mesh_path);
          continue;
        }

        auto texture_id = assets_manager.try_get_asset_id(std::filesystem::path{texture_path});

        if (!texture_id) {
          core::logger::warn("Texture '{}' could not be found", texture_path);
          continue;
        }

        _add_or_update_component<scenes::static_mesh>(entity, *mesh_id, *texture_id);
      } else if (component_type == "Camera") {
        const auto fov = component_node["fov"].as<std::float_t>();
        const auto near = component_node["near"].as<std::float_t>();
        const auto far = component_node["far"].as<std::float_t>();

        auto& window = devices_module.window();

        _add_or_update_component<scenes::camera>(entity, math::degree{fov}, window.aspect_ratio(), near, far);

        _camera = entity;
      } else if (component_type == "Script") {
        const auto path = component_node["script"].as<std::string>();

        _add_or_update_component<scenes::script>(entity, path);
      } else if (component_type == "PointLight") {
        const auto ambient = component_node["ambient"].as<math::color>();
        const auto diffuse = component_node["diffuse"].as<math::color>();
        const auto specular = component_node["specular"].as<math::color>();
        const auto constant = component_node["constant"].as<std::float_t>();
        const auto linear = component_node["linear"].as<std::float_t>();
        const auto quadratic = component_node["quadratic"].as<std::float_t>();

        _add_or_update_component<scenes::point_light>(entity, ambient, diffuse, specular, constant, linear, quadratic);
      } else {
        core::logger::warn("Unknown component type: {}", component_type);
      }
    }
  }

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

} // namespace sbx::scenes
