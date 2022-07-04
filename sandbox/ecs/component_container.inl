#include "component_container.hpp"

#include <functional>

namespace sbx {

template<component Type, typename Allocator>
inline component_container<Type, Allocator>::component_container() noexcept
: _allocator{},
  _sparse{},
  _dense{},
  _components{} {}

template<component Type, typename Allocator>
inline component_container<Type, Allocator>::component_container(component_container&& other) noexcept
: _sparse{std::move(other._sparse)},
  _dense{std::move(other._dense)},
  _components{std::move(other._components)} {}

template<component Type, typename Allocator>
inline component_container<Type, Allocator>& component_container<Type, Allocator>::operator=(component_container&& other) noexcept {
  if (this != &other) {
    _sparse = std::move(other._sparse);
    _dense = std::move(other._dense);
    _components = std::move(other._components);
  }

  return *this;
}

template<component Type, typename Allocator>
inline void component_container<Type, Allocator>::remove(const entity& entity) {
  if (!contains(entity)) {
    return;
  }

  const auto index = _sparse.at(entity);

  _sparse.at(_dense.back()) = index;
  const auto& old_entity = std::exchange(_dense.at(index), _dense.back());

  using std::swap;
  swap(_components.at(index), _components.back());

  _dense.pop_back();
  _components.pop_back();
  _sparse.erase(old_entity);
}

template<component Type, typename Allocator>
inline bool component_container<Type, Allocator>::contains(const entity& entity) const noexcept {
  if (const auto entry = _sparse.find(entity); entry != _sparse.cend()) {
    const auto index = entry->second;
    return index < _dense.size() && _dense[index] == entity;
  }

  return false;
}

template<component Type, typename Allocator>
template<typename... Args>
requires std::constructible_from<Type, Args...>
inline component_container<Type, Allocator>::reference component_container<Type, Allocator>::add(const entity& entity, Args&&... args) {
  if (contains(entity)) {
    throw std::runtime_error{"Entity already contains component"};
  }

  const auto index = _components.size();

  _sparse.emplace(std::make_pair(entity, index));
  _dense.push_back(entity);

  auto component = std::unique_ptr<value_type, component_deleter>{allocator_traits::allocate(_allocator, 1), component_deleter{_allocator}};
  allocator_traits::construct(_allocator, component.get(), std::forward<Args>(args)...);

  _components.push_back(std::move(component));

  return *_components.back();
}

template<component Type, typename Allocator>
inline component_container<Type, Allocator>::const_reference component_container<Type, Allocator>::get(const entity& entity) const {
  if (const auto entry = _sparse.find(entity); entry != _sparse.cend()) {
    const auto index = entry->second;

    return *_components.at(index);
  }

  throw std::runtime_error{"Entity does not have component assigned to it"};
}

template<component Type, typename Allocator>
inline component_container<Type, Allocator>::reference component_container<Type, Allocator>::get(const entity& entity) {
  return const_cast<reference>(std::as_const(*this).get(entity));
}

template<component Type, typename Allocator>
template<std::invocable<Type&> Function>
inline void component_container<Type, Allocator>::patch(const entity& entity, Function&& function) {
  std::invoke(function, get(entity));
}

template<component Type, typename Allocator>
inline component_container<Type, Allocator>::component_deleter::component_deleter(allocator_type& allocator)
: _allocator{std::addressof(allocator)} { }

template<component Type, typename Allocator>
inline void component_container<Type, Allocator>::component_deleter::operator()(value_type* component) {
  allocator_traits::destroy(*_allocator, component);
  allocator_traits::deallocate(*_allocator, component, 1);  
}

} // namespace sbx
