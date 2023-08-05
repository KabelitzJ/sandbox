#ifndef LIBSBX_SCENES_SCENE_HPP_
#define LIBSBX_SCENES_SCENE_HPP_

#include <unordered_map>
#include <memory>
#include <utility>

#include <libsbx/ecs/registry.hpp>
#include <libsbx/ecs/entity.hpp>

#include <libsbx/scenes/node.hpp>

#include <libsbx/scenes/components/id.hpp>
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
  }

  auto create_node(const transform& transform = transform{}) -> node {
    auto node = scenes::node{&_registry, _registry.create_entity()};

    auto& id = node.add_component<scenes::id>();

    _nodes.insert({id, node});

    node.add_component<scenes::relationship>(_root.get_component<scenes::id>());
    _root.get_component<scenes::relationship>().add_child(id);

    node.add_component<scenes::transform>(transform);

    return node;
  }

private:

  std::unordered_map<math::uuid, node> _nodes;

  ecs::registry _registry;
  node _root;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
