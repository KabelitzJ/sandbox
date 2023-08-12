#ifndef LIBSBX_SCENES_SCENE_HPP_
#define LIBSBX_SCENES_SCENE_HPP_

#include <unordered_map>
#include <memory>
#include <utility>
#include <ranges>
#include <vector>

#include <range/v3/all.hpp>

#include <libsbx/ecs/registry.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/ecs/entity.hpp>

#include <libsbx/scenes/node.hpp>

#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/transform.hpp>

namespace sbx::scenes {

class scene {

public:

  scene()
  : _registry{}, 
    _root{&_registry, _registry.create_entity()} {
    auto& id = _root.add_component<scenes::id>();
    _root.add_component<scenes::relationship>(id);
    _root.add_component<scenes::transform>();

    _nodes.insert({id, _root});
  }

  auto create_node(const std::string& tag = "", const transform& transform = scenes::transform{}) -> node {
    auto node = scenes::node{&_registry, _registry.create_entity()};

    auto& id = node.add_component<scenes::id>();

    _nodes.insert({id, node});

    node.add_component<scenes::relationship>(_root.get_component<scenes::id>());
    _root.get_component<scenes::relationship>().add_child(id);

    node.add_component<scenes::transform>(transform);

    node.add_component<scenes::tag>(!tag.empty() ? tag : scenes::tag{"Node"});

    return node;
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
      core::logger::warn("sbx::scenes", "Node '{}' has invalid parent", node.get_component<scenes::tag>());
    }

    _nodes.erase(id);

    _registry.destroy_entity(node._entity);
  }

  auto world_transform(const node& node) -> math::matrix4x4 {
    auto& transform = node.get_component<scenes::transform>();
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

    auto to_node = std::views::transform([&](auto& entity) { return node{&_registry, entity}; });

    return view | to_node | ranges::to<std::vector>();
  }

private:

  std::unordered_map<math::uuid, node> _nodes;

  ecs::registry _registry;
  node _root;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
