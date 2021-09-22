#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <vector>
#include <array>
#include <limits>
#include <bitset>
#include <memory>

#include <util/sparse_set.hpp>

#include "entity.hpp"
#include "component_container.hpp"

namespace sbx {

class registry {

public:
  using id_type = std::size_t;

  registry();
  ~registry();

  [[nodiscard]] entity create_entity();
  
  void destoy_entity(entity entity);

  template<typename Component, typename... Args>
  Component& add_component(entity entity, Args&&... args);

  template<typename Component>
  void remove_component(entity entity);

private:
  template<typename Component>
  id_type _component_id();

  id_type _component_id_counter;

  std::vector<std::unique_ptr<basic_component_container>> _containers;
}; // class registry


template<typename Component, typename... Args>
inline Component& registry::add_component(entity entity, Args&&... args) {
  const auto component_id = _component_id<Component>();

  if (component_id >= _containers.size()) {
    _containers.resize(component_id - 1);
  }

  auto container = *static_cast<component_container<Component>*>(_containers.at(component_id).get());

  return container.emplace_at(entity, std::forward<Args>(args)...);
}

template<typename Component>
inline registry::id_type registry::_component_id() {
  static id_type id = _component_id_counter++;

  return id;
}

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
