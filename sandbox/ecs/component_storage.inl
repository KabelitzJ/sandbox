#include <math/functional.hpp>

namespace sbx {

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_storage<Entity, Component, Allocator>::basic_component_storage()
: _page_allocator{},
  _dense{} { }

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_storage<Entity, Component, Allocator>::basic_component_storage(const allocator_type& allocator)
: _page_allocator{allocator},
  _dense{allocator} { }

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_storage<Entity, Component, Allocator>::basic_component_storage(basic_component_storage&& other) noexcept
: _page_allocator{std::move(other._page_allocator)},
  _dense{std::move(other._dense)} { }

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_storage<Entity, Component, Allocator>::~basic_component_storage() {
  _shrink_to_fit(size_type{0});
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_storage<Entity, Component, Allocator>& basic_component_storage<Entity, Component, Allocator>::operator=(basic_component_storage&& other) noexcept {
  _shrink_to_size(size_type{0});

  base_type::operator=(std::move(other));
  _page_allocator = std::move(other._page_allocator);
  _dense = std::move(other._dense);

  return *this;
}

template<entity Entity, component Component, allocator<Component> Allocator>
basic_component_storage<Entity, Component, Allocator>::reference basic_component_storage<Entity, Component, Allocator>::_element_at(const size_type index) const {
  return _dense[index / page_size][fast_mod(index, page_size)];
}

template<entity Entity, component Component, allocator<Component> Allocator>
basic_component_storage<Entity, Component, Allocator>::pointer basic_component_storage<Entity, Component, Allocator>::_assure_at_least(const size_type index) {
  const auto page = index / page_size;

  if (page >= _dense.size()) {
    auto current_size = _dense.size();
    _dense.resize(page + size_type{1}, nullptr);

    try {
      for (const auto last = _dense.size(); current_size < last; ++current_size) {
        _dense[current_size] = allocator_traits::allocate(_page_allocator, page_size);
      }
    } catch(...) {
      _dense.resize(current_size);
      throw;
    }
  }

  return _dense[page] + fast_mod(index, page_size);
}

template<entity Entity, component Component, allocator<Component> Allocator>
template<typename... Args>
requires (std::is_constructible_v<Component, Args...>)
basic_component_storage<Entity, Component, Allocator>::reference basic_component_storage<Entity, Component, Allocator>::_try_emplace(const entity_type entity, Args&&... args) {
  const auto iterator = base_type::_try_insert(entity_traits::to_underlying(entity));

  try {
    auto element = _assure_at_least(iterator.index());
    allocator_traits::construct(_page_allocator, element, std::forward<Args>(args)...);
    return *element;
  } catch(...) {
    base_type::_swap_and_pop(iterator, iterator + difference_type{1});
    throw;
  }
}

template<entity Entity, component Component, allocator<Component> Allocator>
void basic_component_storage<Entity, Component, Allocator>::_shrink_to_fit(const size_type size) {
  for (auto position = size, length = base_type::size(); position < length; ++position) {
    std::destroy_at(std::addressof(_element_at(position)));
  }

  const auto from = (size + page_size - 1) / page_size;

  for (auto position = from, last = _dense.size(); position < last; ++position) {
    allocator_traits::deallocate(_page_allocator, _dense[position], page_size);
  }

  _dense.resize(from);
}

} // namespace sbx
