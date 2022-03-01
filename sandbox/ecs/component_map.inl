#include <math/functional.hpp>

namespace sbx {

namespace detail {

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize>::component_map_iterator(const container_type& container, const difference_type offset) noexcept
: _container{std::addressof(container)},
  _offset{offset} { }

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize>& component_map_iterator<Container, PageSize>::operator++() noexcept {
  --_offset;
  return *this;
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize> component_map_iterator<Container, PageSize>::operator++(int) noexcept {
  const auto copy = component_map_iterator{*this};
  return ++copy;
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize>& component_map_iterator<Container, PageSize>::operator--() noexcept {
  ++_offset;
  return *this;
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize> component_map_iterator<Container, PageSize>::operator--(int) noexcept {
  const auto copy = component_map_iterator{*this};
  return --copy;
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize>& component_map_iterator<Container, PageSize>::operator+=(const difference_type offset) noexcept {
  _offset -= offset;
  return *this;
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize> component_map_iterator<Container, PageSize>::operator+(const difference_type offset) const noexcept {
  const auto copy = component_map_iterator{*this};
  return copy += offset;
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize>& component_map_iterator<Container, PageSize>::operator-=(const difference_type offset) noexcept {
  _offset += offset;
  return *this;
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize> component_map_iterator<Container, PageSize>::operator-(const difference_type offset) const noexcept {
  const auto copy = component_map_iterator{*this};
  return copy -= offset;
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize>::reference component_map_iterator<Container, PageSize>::operator*() const {
  const auto position = index();
  const auto page = position / page_size;
  const auto data = _container->data();

  return data[page][fast_mod(position, page_size)];
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize>::pointer component_map_iterator<Container, PageSize>::operator->() const {
  const auto position = index();
  const auto page = position / page_size;
  const auto data = _container->data();

  return data[page] + fast_mod(position, page_size);
}

template<container Container, std::size_t PageSize>
inline component_map_iterator<Container, PageSize>::size_type component_map_iterator<Container, PageSize>::index() const noexcept {
  return static_cast<size_type>(_offset) - size_type{1};
}

template<container Container, std::size_t PageSize>
constexpr bool operator==(const component_map_iterator<Container, PageSize>& lhs, const component_map_iterator<Container, PageSize>& rhs) noexcept {
  return lhs.index() == rhs.index();
}

template<container Container, std::size_t PageSize>
constexpr std::strong_ordering operator<=>(const component_map_iterator<Container, PageSize>& lhs, const component_map_iterator<Container, PageSize>& rhs) noexcept {
  return lhs.index() <=> rhs.index();
}

} // namespace detail

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>::basic_component_map()
: base_type{},
  _page_allocator{},
  _dense{} { }

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>::basic_component_map(const allocator_type& allocator)
: base_type{allocator},
  _page_allocator{allocator},
  _dense{allocator} { }

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>::basic_component_map(basic_component_map&& other) noexcept
: base_type{std::move(other)},
  _page_allocator{std::move(other._page_allocator)},
  _dense{std::move(other._dense)} { }

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>::~basic_component_map() {
  _shrink_to_fit(size_type{0});
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>& basic_component_map<Entity, Component, Allocator>::operator=(basic_component_map&& other) noexcept {
  _shrink_to_size(size_type{0});

  base_type::operator=(std::move(other));
  _page_allocator = std::move(other._page_allocator);
  _dense = std::move(other._dense);

  return *this;
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline constexpr basic_component_map<Entity, Component, Allocator>::allocator_type basic_component_map<Entity, Component, Allocator>::get_allocator() const noexcept {
  return _page_allocator;
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline constexpr basic_component_map<Entity, Component, Allocator>::size_type basic_component_map<Entity, Component, Allocator>::size() const noexcept {
  return base_type::size();
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline constexpr bool basic_component_map<Entity, Component, Allocator>::empty() const noexcept {
  return base_type::empty();
}

template<entity Entity, component Component, allocator<Component> Allocator>
template<typename... Args>
requires (std::is_constructible_v<Component, Args...>)
inline basic_component_map<Entity, Component, Allocator>::value_type& basic_component_map<Entity, Component, Allocator>::emplace(const entity_type entity, Args&&... args) {
  if constexpr (std::is_aggregate_v<value_type>) {
    const auto entry = _emplace_component(entity, value_type{std::forward<Args>(args)...});
    return _element_at(static_cast<size_type>(entry.index()));
  } else {
    const auto entry = _emplace_component(entity, std::forward<Args>(args)...);
    return _element_at(static_cast<size_type>(entry.index()));
  }
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline const basic_component_map<Entity, Component, Allocator>::value_type& basic_component_map<Entity, Component, Allocator>::get(const entity_type entity) const {
  if (!base_type::contains(entity)) {
    throw std::out_of_range{"entity does not exist"};
  }
  
  return _element_at(base_type::index(entity));
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>::value_type& basic_component_map<Entity, Component, Allocator>::get(const entity_type entity) {
  return const_cast<value_type&>(std::as_const(*this).get(entity));
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline std::tuple<const typename basic_component_map<Entity, Component, Allocator>::value_type&> basic_component_map<Entity, Component, Allocator>::get_as_tuple(const entity_type entity) const {
  std::forward_as_tuple(get(entity));
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline std::tuple<typename basic_component_map<Entity, Component, Allocator>::value_type&> basic_component_map<Entity, Component, Allocator>::get_as_tuple(const entity_type entity) {
  std::forward_as_tuple(get(entity));
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline void basic_component_map<Entity, Component, Allocator>::_swap_and_pop(base_type_iterator first, base_type_iterator last) {
  for (; first != last; ++first) {
    auto& element = _element_at(base_type::size() - size_type{1});
    std::exchange(_element_at(static_cast<size_type>(first.index())), std::move(element));
    std::destroy_at(std::addressof(element));
    base_type::_swap_and_pop(first, first + difference_type{1});
  }
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>::base_type_iterator basic_component_map<Entity, Component, Allocator>::_try_insert(const entity_type entity) {
  return _emplace_component(entity);
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>::reference basic_component_map<Entity, Component, Allocator>::_element_at(const size_type index) const {
  return _dense[index / page_size][fast_mod(index, page_size)];
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline basic_component_map<Entity, Component, Allocator>::pointer basic_component_map<Entity, Component, Allocator>::_assure_at_least(const size_type index) {
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
inline basic_component_map<Entity, Component, Allocator>::base_type::const_iterator basic_component_map<Entity, Component, Allocator>::_emplace_component(const entity_type entity, Args&&... args) {
  const auto entry = base_type::_try_insert(entity);

  try {
    auto element = _assure_at_least(entry.index());
    allocator_traits::construct(_page_allocator, element, std::forward<Args>(args)...);
  } catch(...) {
    base_type::_swap_and_pop(entry, entry + difference_type{1});
    throw;
  }

  return entry;
}

template<entity Entity, component Component, allocator<Component> Allocator>
inline void basic_component_map<Entity, Component, Allocator>::_shrink_to_fit(const size_type size) {
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
