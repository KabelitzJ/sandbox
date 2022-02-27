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

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::basic_entity_set()
: _sparse{},
  _dense{},
  _free_list{placeholder} { }

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::basic_entity_set(const allocator_type& allocator)
: _sparse{allocator},
  _dense{allocator},
  _free_list{placeholder} { }

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::basic_entity_set(basic_entity_set&& other) noexcept
: _sparse{std::move(other._sparse)},
  _dense{std::move(other._dense)},
  _free_list{std::exchange(other._free_list, placeholder)} { }

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>& basic_entity_set<Type, Allocator>::operator=(basic_entity_set&& other) noexcept {
  _release_sparse_pages();

  _sparse = std::move(other._sparse);
  _dense = std::move(other._dense);
  _free_list = std::exchange(other._free_list, placeholder);

  return *this;
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::~basic_entity_set() {
  _release_sparse_pages();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::size_type basic_entity_set<Type, Allocator>::size() const noexcept {
  return _dense.size();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline bool basic_entity_set<Type, Allocator>::empty() const noexcept {
  return _dense.empty();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_pointer basic_entity_set<Type, Allocator>::data() const noexcept {
  return _dense.data();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_iterator basic_entity_set<Type, Allocator>::begin() const noexcept {
  const auto position = static_cast<difference_type>(_dense.size());
  return const_iterator{_dense, position};
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_iterator basic_entity_set<Type, Allocator>::cbegin() const noexcept {
  return begin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_iterator basic_entity_set<Type, Allocator>::end() const noexcept {
  return const_iterator{_dense, difference_type{0}};
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_iterator basic_entity_set<Type, Allocator>::cend() const noexcept {
  return end();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_iterator basic_entity_set<Type, Allocator>::find(const Type value) const noexcept {
  return contains(value) ? --(cend() - index(value)) : cend();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline bool basic_entity_set<Type, Allocator>::contains(const value_type value) const noexcept {
  const auto element = _sparse_pointer(value);
  
  return element && *element < _dense.size() && _dense[*element] == value;
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::size_type basic_entity_set<Type, Allocator>::index(const value_type value) const noexcept {
  SBX_ASSERT(contains(value), "Value is not in the set");
  return static_cast<size_type>(_sparse_reference(value));
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_reference basic_entity_set<Type, Allocator>::at(const size_type index) const {
  if (index >= _dense.size()) {
    throw std::out_of_range{"Index is out of range"};
  }

  return _dense[index];
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_reference basic_entity_set<Type, Allocator>::operator[](const size_type index) const noexcept {
  SBX_ASSERT(index < _dense.size(), "Index is out of range");
  return _dense[index];
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_entity_set<Type, Allocator>::const_iterator basic_entity_set<Type, Allocator>::insert(const value_type value) {
  return _try_insert(value);
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
void basic_entity_set<Type, Allocator>::remove(const value_type value) {
  if (!contains(value)) {
    return;
  }

  const auto index = --(cend() - index(value));

  _swap_and_pop(index, index + difference_type{1});
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline void basic_entity_set<Type, Allocator>::_swap_and_pop(const_iterator first, const_iterator last) {
  for (; first != last; ++first) {
    _sparse_reference(_dense.back()) = static_cast<value_type>(first.index());
    const auto entry = std::exchange(_dense[first.index()], _dense.back());

    _dense.back() = placeholder;

    _sparse_reference(entry) = placeholder;
    _dense.pop_back();
  }
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::const_iterator basic_entity_set<Type, Allocator>::_try_insert(const value_type value) {
  SBX_ASSERT(!contains(value), "Value already exists in set");

  if (auto& element = _assure_at_least(value); _free_list == placeholder) {
    element = static_cast<value_type>(_dense.size()); 
    _dense.push_back(value);

    return begin();
  } else {
    const auto position = static_cast<size_type>(value);
    element = _free_list;
    _free_list = std::exchange(_dense[position], value);
    return --(end() - static_cast<difference_type>(position));
  }
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::pointer basic_entity_set<Type, Allocator>::_sparse_pointer(const value_type value) const {
  const auto position = static_cast<size_type>(value);
  const auto page = position / page_size;

  return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + fast_mod(position, page_size)) : nullptr;
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::reference basic_entity_set<Type, Allocator>::_sparse_reference(const value_type value) const {
  SBX_ASSERT(_sparse_pointer(value), "Value is not in the set.");

  const auto position = static_cast<size_type>(value);
  const auto page = position / page_size;

  return _sparse[page][fast_mod(position, page_size)];
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline basic_entity_set<Type, Allocator>::reference basic_entity_set<Type, Allocator>::_assure_at_least(const value_type value) {
  const auto position = static_cast<size_type>(value);
  const auto page = position / page_size;

  if (page >= _sparse.size()) {
    _sparse.resize(page + size_type{1}, nullptr);
  }

  if (!_sparse[page]) {
    auto page_allocator = allocator_type{_dense.get_allocator()};
    _sparse[page] = allocator_traits::allocate(page_allocator, page_size);
    std::uninitialized_fill_n(_sparse[page], page_size, placeholder); 
  }

  return _sparse[page][fast_mod(position, page_size)];
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
inline void basic_entity_set<Type, Allocator>::_release_sparse_pages() {
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