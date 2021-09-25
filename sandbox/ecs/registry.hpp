#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <vector>
#include <array>
#include <limits>
#include <bitset>
#include <memory>

#include "entity.hpp"
#include "component_container.hpp"

namespace sbx {

class registry {

public:
  using id_type = std::size_t;

  registry();
  ~registry();

  [[nodiscard]] entity create_entity();
  
  void destoy_entity(const entity& entity);

  template<typename Component, typename... Args>
  void assign_component(const entity entity, Args&&... args);

  template<typename Component>
  void remove_component(const entity entity);

private:
  template<typename Component>
  id_type _component_id();

  id_type _component_id_counter;

  std::vector<std::unique_ptr<basic_component_container>> _components;
}; // class registry


template<typename Component, typename... Args>
inline void registry::assign_component(const entity entity, Args&&... args) {
  const auto component_id = _component_id<Component>();

  if (component_id >= _components.size()) {
    _components.resize(component_id + 1);
  }

  auto& basic_container = _components.at(component_id);

  if (!basic_container.get()) {
    basic_container.reset(new component_container<Component>{});
  }

  // [NOTE] KAJ 2021-09-24 17:40: Cant cast to template class
  // auto container = *static_cast<component_container<Component>*>(basic_container.get());

  // container.assign(entity, std::forward<Args>(args)...);
}

template<typename Component>
void registry::remove_component(const entity entity) {
  const auto component_id = _component_id<Component>();

  if (component_id >= _components.size()) {
    return;
  }

  auto container = *static_cast<component_container<Component>*>(_components.at(component_id).get());

  container.remove(entity);
}

template<typename Component>
inline registry::id_type registry::_component_id() {
  static id_type id = _component_id_counter++;

  return id;
}

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
