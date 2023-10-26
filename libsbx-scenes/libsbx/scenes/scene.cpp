#include <libsbx/scenes/scene.hpp>

#include <libsbx/devices/devices_module.hpp>
#include <libsbx/devices/window.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/script.hpp>

namespace sbx::scenes {

scene::scene()
: _registry{}, 
  _root{&_registry, _registry.create_entity(), &_on_component_added, &_on_component_removed},
  _camera{&_registry, _registry.create_entity(), &_on_component_added, &_on_component_removed} {
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

  auto& camera = _camera.add_component<scenes::camera>(math::angle{math::radian{45.0f}}, 1.0f, 0.1f, 100.0f, true);
  camera.set_aspect_ratio(window.aspect_ratio());

  window.on_framebuffer_resized() += [&camera](const devices::framebuffer_resized_event& event) {
    camera.set_aspect_ratio(static_cast<std::float_t>(event.width) / static_cast<std::float_t>(event.height));
  };
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
  auto node = scenes::node{&_registry, _registry.create_entity(), &_on_component_added, &_on_component_removed};

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
