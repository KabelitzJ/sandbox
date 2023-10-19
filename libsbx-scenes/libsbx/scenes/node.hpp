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

  using signal_container = std::unordered_map<std::type_index, signals::signal<node&>>;

public:

  ~node() = default;

  template<typename Component, typename... Args>
  auto add_component(Args&&... args) -> Component& {
    auto& component = _registry->add_component<Component>(_entity, std::forward<Args>(args)...);

    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _on_component_added->find(type); entry != _on_component_added->end()) {
      entry->second.emit(*this);
    }

    return component;
  }

  template<typename Component>
  auto get_component() -> Component& {
    return _registry->get_component<Component>(_entity);
  }

  template<typename Component>
  auto get_component() const -> const Component& {
    return _registry->get_component<Component>(_entity);
  }

  template<typename Component>
  auto has_component() const -> bool {
    return _registry->has_component<Component>(_entity);
  }

  template<typename Component>
  auto remove_component() -> void {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _on_component_removed->find(type); entry != _on_component_removed->end()) {
      entry->second.emit(*this);
    }

    _registry->remove_component<Component>(_entity);
  }

private:

  node(memory::observer_ptr<ecs::registry> registry, ecs::entity entity, memory::observer_ptr<signal_container> on_component_added, memory::observer_ptr<signal_container> on_component_removed)
  : _registry{registry},
    _entity{entity},
    _on_component_added{on_component_added},
    _on_component_removed{on_component_removed} { }

  memory::observer_ptr<ecs::registry> _registry;
  ecs::entity _entity;

  memory::observer_ptr<signal_container> _on_component_added;
  memory::observer_ptr<signal_container> _on_component_removed;

}; // class node

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_NODE_HPP_
