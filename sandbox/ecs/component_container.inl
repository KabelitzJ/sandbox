#include "component_container.hpp"

#include <functional>

namespace sbx {

template<component Type, typename Allocator>
inline component_container<Type, Allocator>::component_container() noexcept
: base_type{},
  _allocator{},
  _components{} { }

template<component Type, typename Allocator>
template<typename... Args>
requires std::constructible_from<Type, Args...>
inline component_container<Type, Allocator>::reference component_container<Type, Allocator>::add(const entity& entity, Args&&... args) {
  if (contains(entity)) {
    throw std::runtime_error{"Entity already contains component"};
  }

  base_type::_emplace(entity);

  auto component = std::unique_ptr<value_type, component_deleter>{allocator_traits::allocate(_allocator, 1), component_deleter{_allocator}};
  allocator_traits::construct(_allocator, component.get(), std::forward<Args>(args)...);

  _components.push_back(std::move(component));

  return *_components.back();
}

template<component Type, typename Allocator>
inline component_container<Type, Allocator>::const_reference component_container<Type, Allocator>::get(const entity& entity) const {
  return *_components.at(_index(entity));
}

template<component Type, typename Allocator>
inline component_container<Type, Allocator>::reference component_container<Type, Allocator>::get(const entity& entity) {
  return *_components.at(_index(entity));
}

template<component Type, typename Allocator>
template<std::invocable<Type&> Function>
inline void component_container<Type, Allocator>::patch(const entity& entity, Function&& function) {
  std::invoke(function, get(entity));
}

template<component Type, typename Allocator>
inline void component_container<Type, Allocator>::_swap_and_pop(const entity& entity) {
  const auto index = _index(entity);

  using std::swap;
  swap(_components[index], _components.back());

  _components.pop_back();

  base_type::_swap_and_pop(entity);
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
