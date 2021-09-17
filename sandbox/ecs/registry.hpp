#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <vector>
#include <array>
#include <limits>
#include <bitset>

#include <util/fixed_object_pool.hpp>

#include "entity.hpp"

namespace sbx {

class registry {

public:
  using id_type = std::size_t;

  static constexpr id_type MAX_COMPONENTS{64};
  static constexpr id_type MAX_ENTITIES{2048};
  static constexpr id_type INVALID_ENTITY{2049};

  registry();
  ~registry();

  [[nodiscard]] entity create_entity();
  
  void destoy_entity(const entity& entity);

  template<typename Component, typename... Args>
  Component* add_component(const entity& entity, Args&&... args);

  template<typename Component>
  void remove_component(const entity& entity);

  template<typename Component>
  Component* get_component(const entity& entity);

  template<typename Component>
  bool has_component(const entity& entity);

private:
  template<typename Component>
  id_type _component_id();

  id_type _component_id_counter;

  std::vector<entity> _entities;
  std::vector<fixed_object_pool> _component_pools;
  std::vector<std::bitset<MAX_COMPONENTS>> _component_masks;

}; // class registry

// (TODO) KAJ 2021-09-12: Add check if component already exists on entity
template<typename Component, typename... Args>
inline Component* registry::add_component(const entity& entity, Args&&... args) {
  const auto component_id = _component_id<Component>();

  if (component_id >= _component_pools.size()) {
    _component_pools.emplace_back(MAX_ENTITIES, sizeof(Component));
  }

  // const auto component = _component_pools[component_id].construct<Component>(entity, std::forward<Args>(args)...);

  fixed_object_pool pool = _component_pools[component_id];

  const auto component = pool.construct<Component>(entity, std::forward<Args>(args)...);

  _component_masks[entity].set(component_id, true);

  return component;
}

// (TODO) KAJ 2021-09-12: Add check if component does exists on entity
template<typename Component>
inline void registry::remove_component(const entity& entity) {
  const auto component_id = _component_id<Component>();

  fixed_object_pool pool = _component_pools[component_id];

  pool.destroy<Component>(entity);

  _component_masks[entity].set(component_id, false);
}

template<typename Component>
inline Component* registry::get_component(const entity& entity) {
  const auto component_id = _component_id<Component>();

  if (!_component_masks[entity].test(component_id)) {
    return nullptr;
  }

  fixed_object_pool pool = _component_pools[component_id];

  return pool.get<Component>(entity);
}

template<typename Component>
inline bool registry::has_component(const entity& entity) {
  const auto component_id = _component_id<Component>();

  fixed_object_pool pool = _component_pools[component_id];

  return pool.get<Component>(entity) != nullptr;
}

template<typename Component>
inline registry::id_type registry::_component_id() {
  static id_type id = _component_id_counter++;

  return id;
}

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
