#ifndef LIBSBX_SCENES_SCENE_HPP_
#define LIBSBX_SCENES_SCENE_HPP_

#include <unordered_map>
#include <memory>
#include <utility>
#include <ranges>
#include <vector>
#include <typeindex>

#include <range/v3/all.hpp>

#include <yaml-cpp/yaml.h>

#include <libsbx/ecs/registry.hpp>
#include <libsbx/ecs/entity.hpp>

#include <libsbx/math/transform.hpp>
#include <libsbx/math/angle.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/scenes/node.hpp>

#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/script.hpp>

namespace sbx::scenes {

class scene {

  friend class node;

  using signal_container = std::unordered_map<std::type_index, signals::signal<node&>>;

public:

  scene()
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
  }

  auto start() -> void {
    auto script_nodes = query<scenes::script>();

    for (auto& node : script_nodes) {
      auto& script = node.get_component<scenes::script>();

      auto& transform = node.get_component<math::transform>();

      script.set("transform", transform);

      script.invoke("on_create");

      transform = script.get<math::transform>("transform");
    }
  }

  auto create_child_node(node& parent, const std::string& tag = "", const math::transform& transform = math::transform{}) -> node {
    auto node = scenes::node{&_registry, _registry.create_entity(), &_on_component_added, &_on_component_removed};

    auto& id = node.add_component<scenes::id>();

    _nodes.insert({id, node});

    node.add_component<scenes::relationship>(parent.get_component<scenes::id>());
    parent.get_component<scenes::relationship>().add_child(id);

    node.add_component<math::transform>(transform);

    node.add_component<scenes::tag>(!tag.empty() ? tag : scenes::tag{"Node"});

    return node;
  }

  auto create_node(const std::string& tag = "", const math::transform& transform = math::transform{}) -> node {
    return create_child_node(_root, tag, transform);
  }

  auto destroy_node(const node& node) -> void {
    auto& id = node.get_component<scenes::id>();
    auto& relationship = node.get_component<scenes::relationship>();

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

  // auto create_camera(const math::angle& field_of_view, std::float_t aspect_ratio, std::float_t near_plane, std::float_t far_plane, const std::string& tag = "", const math::transform& transform = math::transform{}, bool is_active = true) -> node {
  //   auto node = create_node(tag, transform);

  //   if (is_active) {
  //     auto nodes = query<scenes::camera>();

  //     for (auto& node : nodes) {
  //       auto& camera = node.get_component<scenes::camera>();
  //       camera.set_is_active(false);
  //     }

  //     _camera = node;
  //   }

  //   node.add_component<scenes::camera>(field_of_view, aspect_ratio, near_plane, far_plane, is_active);

  //   return node;
  // }

  auto camera() -> node {
    return _camera;
  }

  auto world_transform(const node& node) -> math::matrix4x4 {
    auto& transform = node.get_component<math::transform>();
    auto& relationship = node.get_component<scenes::relationship>();

    auto& parent = _nodes.at(relationship.parent());

    auto world = math::matrix4x4::identity;

    if (parent.get_component<scenes::id>() != _root.get_component<scenes::id>()) {
      world = world_transform(parent);
    }

    return world * transform.as_matrix();
  }

  template<typename... Components>
  auto query() -> std::vector<node> {
    auto view = _registry.create_view<Components...>();

    auto to_node = std::views::transform([&](auto& entity) { return node{&_registry, entity, &_on_component_added, &_on_component_removed}; });

    return view | to_node | ranges::to<std::vector>();
  }

  template<typename Component>
  auto on_component_added() -> signals::signal<node&>& {
    const auto type = std::type_index{typeid(Component)};

    return _on_component_added[type];
  }

  template<typename Component>
  auto on_component_removed() -> signals::signal<node&>& {
    const auto type = std::type_index{typeid(Component)};

    return _on_component_removed[type];
  }

private:

  signal_container _on_component_added;
  signal_container _on_component_removed;

  std::unordered_map<math::uuid, node> _nodes;

  ecs::registry _registry;
  node _root;
  node _camera;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
