#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <memory>
#include <unordered_map>

#include <meta/concepts.hpp>

#include <types/primitives.hpp>

#include <utils/type_id.hpp>

#include "component_map.hpp"
#include "entity_set.hpp"
#include "entity_traits.hpp"

namespace sbx {

template<entity Entity, allocator<Entity> Allocator>
class basic_registry final {

  using allocator_traits = std::allocator_traits<Allocator>;
  using entity_traits =  entity_traits<Entity>;

  using common_container_type = basic_entity_set<Entity, Allocator>;

  template<component Component>
  using component_container_type = basic_component_map<Entity, Component, typename std::allocator_traits<Allocator>::rebind_alloc<Component>>;

  using version_type = entity_traits::version_type;

public:

  using entity_type = Entity;
  using allocator_type = Allocator;
  using size_type = std::size_t;

  basic_registry() noexcept;

  explicit basic_registry(const allocator_type& allocator) noexcept;

  basic_registry(const basic_registry& other) = delete;

  basic_registry(basic_registry&& other) noexcept;

  ~basic_registry() = default;

  basic_registry& operator=(const basic_registry& other) = delete;

  basic_registry& operator=(basic_registry&& other) noexcept;

  [[nodiscard]] entity_type create_entity();

private:

  [[nodiscard]] entity_type _generate_entity(const size_type position) const noexcept;

  [[nodiscard]] entity_type _recycle_entity() noexcept;

  version_type _release_entity(const entity_type entity, const version_type version) noexcept;

  std::unordered_map<uint32, std::unique_ptr<common_container_type>> _component_containers{};
  std::vector<entity_type> _entities{};
  entity_type _free_list{};

}; // class basic_registry

template<entity Entity>
using registry = basic_registry<Entity, std::allocator<Entity>>;

} // namespace sbx

#include "registry.inl"

#endif // SBX_ECS_REGISTRY_HPP_
