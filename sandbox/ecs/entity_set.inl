#include <platform/assert.hpp>

#include <math/functional.hpp>

namespace sbx {

namespace detail {

template<container Container>
inline entity_set_iterator<Container>::entity_set_iterator(const container_type& container, const difference_type offset) noexcept
: _container(std::addressof(container)),
  _offset(offset) { }

template<container Container>
inline entity_set_iterator<Container>& entity_set_iterator<Container>::operator++() noexcept {
  --_offset;
  return *this;
}

template<container Container>
inline entity_set_iterator<Container> entity_set_iterator<Container>::operator++(int) noexcept {
  const auto copy = entity_set_iterator{*this};
  return --copy;
}

template<container Container>
inline entity_set_iterator<Container>& entity_set_iterator<Container>::operator--() noexcept {
  ++_offset;
  return *this;
}

template<container Container>
inline entity_set_iterator<Container> entity_set_iterator<Container>::operator--(int) noexcept {
  const auto copy = entity_set_iterator{*this};
  return ++copy;
}

template<container Container>
inline entity_set_iterator<Container>& entity_set_iterator<Container>::operator+=(const difference_type offset) noexcept {
  _offset -= offset;
  return *this;
}

template<container Container>
inline entity_set_iterator<Container> entity_set_iterator<Container>::operator+(const difference_type offset) const noexcept {
  auto copy = entity_set_iterator{*this};
  return copy -= offset;
}

template<container Container>
inline entity_set_iterator<Container>& entity_set_iterator<Container>::operator-=(const difference_type offset) noexcept {
  _offset += offset;
  return *this;
}

template<container Container>
inline entity_set_iterator<Container> entity_set_iterator<Container>::operator-(const difference_type offset) const noexcept {
  auto copy = entity_set_iterator{*this};
  return copy += offset;
}

template<container Container>
inline entity_set_iterator<Container>::reference entity_set_iterator<Container>::operator*() const {
  const auto data = _container->data();
  return data[index()];
}

template<container Container>
inline entity_set_iterator<Container>::pointer entity_set_iterator<Container>::operator->() const {
  const auto data = _container->data();
  return data + index();
}

template<container Container>
inline entity_set_iterator<Container>::size_type entity_set_iterator<Container>::index() const noexcept {
  return static_cast<size_type>(_offset) - size_type{1};
}

template<container Container>
inline bool operator==(const entity_set_iterator<Container>& lhs, const entity_set_iterator<Container>& rhs) noexcept {
  return lhs.index() == rhs.index();
}

template<container Container>
inline std::strong_ordering operator<=>(const entity_set_iterator<Container>& lhs, const entity_set_iterator<Container>& rhs) noexcept {
  return lhs.index() <=> rhs.index();
}

} // namespace detail

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::basic_entity_set()
: _sparse{},
  _dense{},
  _free_list{tombstone_entity} { }

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::basic_entity_set(const allocator_type& allocator)
: _sparse{allocator},
  _dense{allocator},
  _free_list{tombstone_entity} { }

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::basic_entity_set(basic_entity_set&& other) noexcept
: _sparse{std::move(other._sparse)},
  _dense{std::move(other._dense)},
  _free_list{std::exchange(other._free_list, tombstone_entity)} { }

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>& basic_entity_set<Entity, Allocator>::operator=(basic_entity_set&& other) noexcept {
  _release_sparse_pages();

  _sparse = std::move(other._sparse);
  _dense = std::move(other._dense);
  _free_list = std::exchange(other._free_list, tombstone_entity);

  return *this;
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::~basic_entity_set() {
  _release_sparse_pages();
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::size_type basic_entity_set<Entity, Allocator>::size() const noexcept {
  return _dense.size();
}

template<entity Entity, allocator<Entity> Allocator>
inline bool basic_entity_set<Entity, Allocator>::empty() const noexcept {
  return _dense.empty();
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_pointer basic_entity_set<Entity, Allocator>::data() const noexcept {
  return _dense.data();
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_iterator basic_entity_set<Entity, Allocator>::begin() const noexcept {
  const auto position = static_cast<difference_type>(_dense.size());
  return const_iterator{_dense, position};
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_iterator basic_entity_set<Entity, Allocator>::cbegin() const noexcept {
  return begin();
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_iterator basic_entity_set<Entity, Allocator>::end() const noexcept {
  return const_iterator{_dense, difference_type{0}};
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_iterator basic_entity_set<Entity, Allocator>::cend() const noexcept {
  return end();
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_iterator basic_entity_set<Entity, Allocator>::find(const entity_type entity) const noexcept {
  return contains(entity) ? --(cend() - index(entity)) : cend();
}

template<entity Entity, allocator<Entity> Allocator>
inline bool basic_entity_set<Entity, Allocator>::contains(const entity_type entity) const noexcept {
  const auto element = _sparse_pointer(entity);
  constexpr auto capacity = entity_traits::to_entity(null_entity);
  
  return element && (((~capacity & entity_traits::to_integral(entity)) ^ entity_traits::to_integral(*element)) < capacity);
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::size_type basic_entity_set<Entity, Allocator>::index(const entity_type entity) const noexcept {
  SBX_ASSERT(contains(entity), "entity is not in the set");
  return static_cast<size_type>(entity_traits::to_entity(_sparse_reference(entity)));
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_reference basic_entity_set<Entity, Allocator>::at(const size_type index) const {
  if (index >= _dense.size()) {
    throw std::out_of_range{"Index is out of range"};
  }

  return _dense[index];
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_reference basic_entity_set<Entity, Allocator>::operator[](const size_type index) const noexcept {
  SBX_ASSERT(index < _dense.size(), "Index is out of range");
  return _dense[index];
}

template<entity Entity, allocator<Entity> Allocator>
basic_entity_set<Entity, Allocator>::const_iterator basic_entity_set<Entity, Allocator>::insert(const entity_type entity) {
  return _try_insert(entity);
}

template<entity Entity, allocator<Entity> Allocator>
void basic_entity_set<Entity, Allocator>::remove(const entity_type entity) {
  if (!contains(entity)) {
    return;
  }

  const auto index = --(cend() - index(entity));

  _swap_and_pop(index, index + difference_type{1});
}

template<entity Entity, allocator<Entity> Allocator>
inline void basic_entity_set<Entity, Allocator>::_swap_and_pop(const_iterator first, const_iterator last) {
  for (; first != last; ++first) {
    _sparse_reference(_dense.back()) = entity_traits::combine(static_cast<entity_traits::entity_type>(first.index()), entity_traits::to_integral(_dense.back()));
    const auto entry = std::exchange(_dense[first.index()], _dense.back());

    // Can catch some mugs here, but it's not necessary
    SBX_ASSERT((_dense.back() = tombstone_entity, true));

    _sparse_reference(entry) = null_entity;
    _dense.pop_back();
  }
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::const_iterator basic_entity_set<Entity, Allocator>::_try_insert(const entity_type entity) {
  SBX_ASSERT(!contains(entity), "entity already exists in set");

  if (auto& element = _assure_at_least(entity); _free_list == null_entity) {
    element = entity_traits::combine(static_cast<entity_traits::entity_type>(_dense.size()), entity_traits::to_integral(entity));
    _dense.push_back(entity);

    return begin();
  } else {
    const auto position = static_cast<size_type>(entity_traits::to_entity(_free_list));
    element = entity_traits::combine(entity_traits::to_integral(_free_list), entity_traits::to_integral(entity));
    _free_list = std::exchange(_dense[position], entity);
    return --(end() - static_cast<difference_type>(position));
  }
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::pointer basic_entity_set<Entity, Allocator>::_sparse_pointer(const entity_type entity) const {
  const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
  const auto page = position / page_size;

  return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + fast_mod(position, page_size)) : nullptr;
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::reference basic_entity_set<Entity, Allocator>::_sparse_reference(const entity_type entity) const {
  SBX_ASSERT(_sparse_pointer(entity), "entity is not in the set.");

  const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
  const auto page = position / page_size;

  return _sparse[page][fast_mod(position, page_size)];
}

template<entity Entity, allocator<Entity> Allocator>
inline basic_entity_set<Entity, Allocator>::reference basic_entity_set<Entity, Allocator>::_assure_at_least(const entity_type entity) {
  const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
  const auto page = position / page_size;

  if (page >= _sparse.size()) {
    _sparse.resize(page + size_type{1}, nullptr);
  }

  if (!_sparse[page]) {
    auto page_allocator = allocator_type{_dense.get_allocator()};
    _sparse[page] = allocator_traits::allocate(page_allocator, page_size);
    std::uninitialized_fill_n(_sparse[page], page_size, null_entity); 
  }

  auto& element = _sparse[page][fast_mod(position, page_size)];

  SBX_ASSERT(entity_traits::to_version(element) == entity_traits::to_version(tombstone_entity), "Entity not available");

  return element;
}

template<entity Entity, allocator<Entity> Allocator>
inline void basic_entity_set<Entity, Allocator>::_release_sparse_pages() {
  auto page_allocator = allocator_type{_dense.get_allocator()};

  for (auto* page : _sparse) {
    if (page) {
      std::destroy_n(page, page_size);
      allocator_traits::deallocate(page_allocator, page, page_size);   
      page = nullptr;
    }
  }
}

} // namespace sbx