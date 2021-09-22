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
  
  void destoy_entity(const entity& entity);

  template<typename Component, typename... Args>
  void add_component(const entity& entity, Args&&... args);

private:
  template<typename Component>
  id_type _component_id();

  id_type _component_id_counter;

  std::vector<std::unique_ptr<basic_component_container>> _components;
}; // class registry


template<typename Component, typename... Args>
inline void registry::add_component(const entity& entity, Args&&... args) {
  const auto component_id = _component_id<Component>();

  if (component_id >= _components.size()) {
    _components.push_back(std::make_unique<component_container<Component>>());
  }

  (void)entity;
}

template<typename Component>
inline registry::id_type registry::_component_id() {
  static id_type id = _component_id_counter++;

  return id;
}

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
