#include <libsbx/scenes/scene.hpp>

#include <unordered_map>

// #include <portable-file-dialogs.h>

#include <yaml-cpp/yaml.h>

#include <libsbx/units/time.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/math/angle.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/devices/devices_module.hpp>
#include <libsbx/devices/window.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/point_light.hpp>

namespace sbx::scenes {

scene::scene(const std::filesystem::path& path)
: _registry{}, 
  _root{&_registry, _registry.create()},
  _camera{&_registry, _registry.create()},
  _light{math::vector3{-1.0, -1.0, -1.0}, math::color::white} {
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

  _camera.add_component<scenes::camera>(math::angle{math::degree{50.0f}}, window.aspect_ratio(), 0.1f, 2000.0f);

  window.on_framebuffer_resized() += [this](const devices::framebuffer_resized_event& event) {
    auto& camera = _camera.get_component<scenes::camera>();
    camera.set_aspect_ratio(static_cast<std::float_t>(event.width) / static_cast<std::float_t>(event.height));
  };

  const auto scene = YAML::LoadFile(path.string());

  const auto& name = scene["name"].as<std::string>();

  const auto& metadata = scene["metadata"];

  _load_assets(scene["assets"]);
  _load_nodes(scene["nodes"]);
}

auto scene::create_child_node(node& parent, const std::string& tag, const math::transform& transform) -> node {
  auto node = scenes::node{&_registry, _registry.create()};

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

  for (auto& child_id : relationship.children()) {
    if (auto child = find_node(child_id); child) {
      destroy_node(*child);
    }
  }

  if (auto entry = _nodes.find(id); entry != _nodes.end()) {
    entry->second.get_component<scenes::relationship>().remove_child(id);
  } else {
    core::logger::warn("Node '{}' has invalid parent", node.get_component<scenes::tag>());
  }

  _nodes.erase(id);

  _registry.destroy(node._entity);
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

auto scene::_load_assets(const YAML::Node& assets) -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  for (const auto& mesh : assets["meshes"]) {
    const auto& name = mesh["name"].as<std::string>();
    const auto& path = mesh["path"].as<std::string>();
    const auto& id = mesh["id"].as<std::string>();
  }

  for (const auto& mesh : assets["materials"]) {
    const auto& name = mesh["name"].as<std::string>();
    const auto& path = mesh["path"].as<std::string>();
    const auto& id = mesh["id"].as<std::string>();
  }
}

auto scene::_load_nodes(const YAML::Node& assets) -> void {

}

} // namespace sbx::scenes
