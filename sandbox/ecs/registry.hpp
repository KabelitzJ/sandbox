#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <queue>

#include <container/sparse_set.hpp>

#include <meta/concepts.hpp>

#include <types/primitives.hpp>

#include <utils/type_id.hpp>

#include "component_container.hpp"

namespace sbx {

template<typename Entity, allocator<Entity> Allocator>
class basic_registry {

  using basic_common_container = basic_sparse_set<Entity, Allocator>;
  
  template<typename Component>
  using component_container = basic_component_container<Entity, Component, Allocator>;  

public:

  using entity_type = Entity;

  template<component Component, typename... Args>
  Component& add_component(const entity_type entity, Args&&... args);

  template<component Component>
  void remove_component(const entity_type entity);

private:

  std::unordered_map<uint32, std::unique_ptr<basic_common_container>> _component_pools{};
  std::vector<entity_type> _entities{};
  std::queue<entity_type> _free_list{};

}; // class basic_registry

using registry = basic_registry<uint32, std::allocator<uint32>>;

} // namespace sbx

#include "registry.inl"

#endif // SBX_ECS_REGISTRY_HPP_
