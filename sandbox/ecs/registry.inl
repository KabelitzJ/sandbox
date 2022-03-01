#include <platform/assert.hpp>

namespace sbx {

template<entity Entity, allocator<Entity> Allocator>
inline basic_registry<Entity, Allocator>::basic_registry() noexcept
: _component_containers{},
  _entities{},
  _free_list{tombstone_entity} { }

template<entity Entity, allocator<Entity> Allocator>
inline basic_registry<Entity, Allocator>::basic_registry(const allocator_type& allocator) noexcept
: _component_containers{allocator},
  _entities{allocator},
  _free_list{tombstone_entity} { }

template<entity Entity, allocator<Entity> Allocator>
inline basic_registry<Entity, Allocator>::basic_registry(basic_registry&& other) noexcept
: _component_containers{std::move(other._component_containers)},
  _entities{std::move(other._entities)},
  _free_list{std::exchange(other._free_list, tombstone_entity)} { }
  
template<entity Entity, allocator<Entity> Allocator>
inline basic_registry<Entity, Allocator>& basic_registry<Entity, Allocator>::operator=(basic_registry&& other) noexcept {
  _component_containers = std::move(other._component_containers);
  _entities = std::move(other._entities);
  _free_list = std::exchange(other._free_list, tombstone_entity);

  return *this;
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_registry<Entity, Allocator>::entity_type basic_registry<Entity, Allocator>::create_entity() {
  return (_free_list == null_entity) ? _entities.emplace_back(_generate_entity(_entities.size())) : _recycle_entity();
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_registry<Entity, Allocator>::entity_type basic_registry<Entity, Allocator>::_generate_entity(const size_type position) const noexcept {
  SBX_ASSERT(position < entity_traits::to_integral(null_entity), "No more entities can be generated.");
  return entity_traits::construct(static_cast<entity_traits::entity_type>(position), version_type{0});
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_registry<Entity, Allocator>::entity_type basic_registry<Entity, Allocator>::_recycle_entity() noexcept {
  SBX_ASSERT(_free_list != null_entity, "No entities available");
  const auto current = entity_traits::to_entity(_free_list);
  _free_list = entity_traits::combine(entity_traits::to_integral(_entities[current]), tombstone_entity);
  return (_entities[current] = entity_traits::combine(current, entity_traits::to_integral(_entities[current])));
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_registry<Entity, Allocator>::version_type basic_registry<Entity, Allocator>::_release_entity(const entity_type entity, const version_type version) noexcept {
  const version_type new_version = version + (version == entity_traits::to_version(tombstone_entity));
  _entities[entity_traits::to_entity(entity)] = entity_traits::construct(entity_traits::to_integral(_free_list), new_version);
  _free_list = entity_traits::combine(entity_traits::to_integral(entity), tombstone_entity);
  return new_version;
}

} // namespace sbx