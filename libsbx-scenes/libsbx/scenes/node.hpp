#ifndef LIBSBX_SCENES_NODE_HPP_
#define LIBSBX_SCENES_NODE_HPP_

#include <unordered_set>

#include <libsbx/ecs/registry.hpp>
#include <libsbx/ecs/entity.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/memory/observer_ptr.hpp>

namespace sbx::scenes {

class node {

  friend class scene;

public:

  ~node() = default;

  template<typename Component, typename... Args>
  auto add_component(Args&&... args) -> Component& {
    return _registry->emplace<Component>(_entity, std::forward<Args>(args)...);
  }

  template<typename Component>
  auto get_component() -> Component& {
    return _registry->get<Component>(_entity);
  }

  template<typename Component>
  auto get_component() const -> const Component& {
    return _registry->get<Component>(_entity);
  }

  template<typename Component>
  auto has_component() const -> bool {
    return _registry->try_get<Component>(_entity) != nullptr;
  }

  template<typename Component>
  auto remove_component() -> void {
    _registry->remove<Component>(_entity);
  }

  friend auto operator==(const node& lhs, const node& rhs) -> bool {
    return lhs._registry == rhs._registry && lhs._entity == rhs._entity;
  }

private:

  node(memory::observer_ptr<ecs::registry> registry, ecs::entity entity)
  : _registry{registry},
    _entity{entity} { }

  memory::observer_ptr<ecs::registry> _registry;
  ecs::entity _entity;

}; // class node

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_NODE_HPP_
