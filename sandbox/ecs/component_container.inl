namespace sbx {

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::basic_component_container(const allocator_type& allocator)
: base_type{allocator},
  _dense{allocator} { }

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::basic_component_container(basic_component_container&& other)
: base_type{std::move(other)},
  _dense{std::move(other._dense)} { }

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>& basic_component_container<Entity, Component, Allocator>::operator=(basic_component_container&& other) {
  base_type::operator=(std::move(other));
  _dense = std::move(other._dense);

  return *this;
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::const_iterator basic_component_container<Entity, Component, Allocator>::begin() const noexcept {
  return _dense.cbegin();
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::const_iterator basic_component_container<Entity, Component, Allocator>::cbegin() const noexcept {
  return _dense.cbegin();
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::const_iterator basic_component_container<Entity, Component, Allocator>::end() const noexcept {
  return _dense.cend();
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::const_iterator basic_component_container<Entity, Component, Allocator>::cend() const noexcept {
  return _dense.cend();
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::size_type basic_component_container<Entity, Component, Allocator>::size() const noexcept {
  return _dense.size();
}

template<typename Entity, component Component, allocator<Component> Allocator>
bool basic_component_container<Entity, Component, Allocator>::contains(const entity_type& entity) const noexcept {
  return base_type::contains(entity);
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::reference basic_component_container<Entity, Component, Allocator>::operator[](const entity_type& entity) {
  return _dense[*base_type::find(entity)];
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::const_reference basic_component_container<Entity, Component, Allocator>::operator[](const entity_type& entity) const {
  return _dense[*base_type::find(entity)];
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::reference basic_component_container<Entity, Component, Allocator>::at(const entity_type& entity) {
  return _dense.at(*base_type::find(entity));
}

template<typename Entity, component Component, allocator<Component> Allocator>
basic_component_container<Entity, Component, Allocator>::const_reference basic_component_container<Entity, Component, Allocator>::at(const entity_type& entity) const {
  return _dense.at(*base_type::find(entity));
}

template<typename Entity, component Component, allocator<Component> Allocator>
bool basic_component_container<Entity, Component, Allocator>::empty() const noexcept {
  return _dense.empty();
}

template<typename Entity, component Component, allocator<Component> Allocator>
template<typename... Args>
basic_component_container<Entity, Component, Allocator>::value_type& basic_component_container<Entity, Component, Allocator>::emplace(const entity_type entity, Args&&... args) {
  if (const auto entry = base_type::find(entity); entry != base_type::end()) {
    return _dense[*entry];
  }

  const auto index = *base_type::insert(entity);;

  if (index >= _dense.size()) {
    _dense.resize(index + size_type{1}, value_type{});
  }
  
  _dense[index] = value_type{std::forward<Args>(args)...};

  return _dense[index];
}

template<typename Entity, component Component, allocator<Component> Allocator>
void basic_component_container<Entity, Component, Allocator>::remove(const entity_type entity) {
  if (const auto entry = base_type::find(entity); entry != base_type::end()) {
    const auto index = *entry;

    base_type::erase(index);

    if (index < _dense.size()) {
      std::swap(_dense[index], _dense.back());
      _dense.pop_back();
    }
  }
}

template<typename Entity, component Component, allocator<Component> Allocator>
void basic_component_container<Entity, Component, Allocator>::clear() {
  base_type::clear();
  _dense.clear();
}

} // namespace sbx