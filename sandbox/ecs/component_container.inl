namespace sbx {

template<entity Entity, typename Component, allocator<Component> Allocator, std::size_t PageSize, Entity Placeholder>
basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>::basic_component_container()
: base_type{}, 
  _packed{}, 
  _allocator{} { }

template<entity Entity, typename Component, allocator<Component> Allocator, std::size_t PageSize, Entity Placeholder>
basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>::basic_component_container(const allocator_type& allocator)
: base_type{allocator}, 
  _packed{allocator}, 
  _allocator{allocator} { }

template<entity Entity, typename Component, allocator<Component> Allocator, std::size_t PageSize, Entity Placeholder>
basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>::basic_component_container(const basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>&& other) noexcept
: base_type{std::move(other)},
  _packed{std::move(other._packed)},
  _allocator{std::move(other._allocator)} { }

template<entity Entity, typename Component, allocator<Component> Allocator, std::size_t PageSize, Entity Placeholder>
basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>::~basic_component_container() {
  // [TODO] KAJ 2022-02-19 17:17 - shrink_to_size(size_type{0});
}

template<entity Entity, typename Component, allocator<Component> Allocator, std::size_t PageSize, Entity Placeholder>
basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>& basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>::operator=(const basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>&& other) noexcept {
  // [TODO] KAJ 2022-02-19 17:17 - shrink_to_size(size_type{0});

  base_type::operator=(std::move(other));
  _packed = std::move(other._packed);
  
  if constexpr (allocator_traits::propagate_on_container_move_assignment::value) {
    _allocator = std::move(other._allocator);
  }

  return *this;
}

} // namespace sbx