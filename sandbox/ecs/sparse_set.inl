#include <memory>

#include <platform/assert.hpp>

#include <math/functional.hpp>

namespace sbx {

namespace detail {

template<container Container>
inline sparse_set_iterator<Container>::sparse_set_iterator(const Container& container, const difference_type offset) noexcept
: _container(std::addressof(container)),
  _offset(offset) { }

template<container Container>
inline sparse_set_iterator<Container>& sparse_set_iterator<Container>::operator++() noexcept {
  --_offset;
  return *this;
}

template<container Container>
inline sparse_set_iterator<Container> sparse_set_iterator<Container>::operator++(int) noexcept {
  const auto copy = sparse_set_iterator<Container>{*this};
  --_offset;
  return copy;
}

template<container Container>
inline sparse_set_iterator<Container>& sparse_set_iterator<Container>::operator--() noexcept {
  ++_offset;
  return *this;
}

template<container Container>
inline sparse_set_iterator<Container> sparse_set_iterator<Container>::operator--(int) noexcept {
  const auto copy = sparse_set_iterator<Container>{*this};
  ++_offset;
  return copy;
}

template<container Container>
inline sparse_set_iterator<Container>& sparse_set_iterator<Container>::operator+=(const difference_type offset) noexcept {
  _offset -= offset;
  return *this;
}

template<container Container>
inline sparse_set_iterator<Container> sparse_set_iterator<Container>::operator+(const difference_type offset) const noexcept {
  auto copy = sparse_set_iterator<Container>{*this};
  return copy += offset;
}

template<container Container>
inline sparse_set_iterator<Container>& sparse_set_iterator<Container>::operator-=(const difference_type offset) noexcept {
  _offset += offset;
  return *this;
}

template<container Container>
inline sparse_set_iterator<Container> sparse_set_iterator<Container>::operator-(const difference_type offset) const noexcept {
  auto copy = sparse_set_iterator<Container>{*this};
  return copy -= offset;
}

template<container Container>
inline typename sparse_set_iterator<Container>::reference sparse_set_iterator<Container>::operator[](const size_type index) const {
  return _container->data()[index() - index];
}

template<container Container>
inline typename sparse_set_iterator<Container>::reference sparse_set_iterator<Container>::operator*() const {
  return _container->data() + index();
}

template<container Container>
inline typename sparse_set_iterator<Container>::pointer sparse_set_iterator<Container>::operator->() const {
  return *(_container->data() + index());
}

template<container Container>
inline typename sparse_set_iterator<Container>::difference_type sparse_set_iterator<Container>::index() const noexcept {
  return static_cast<size_type>(_offset - 1);
}

template<container Container>
inline typename sparse_set_iterator<Container>::difference_type operator-(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept {
  return lhs.index() - rhs.index();
}

template<container Container>
inline bool operator==(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept {
  return lhs.index() == rhs.index();
}

template<container Container>
inline std::strong_ordering operator<=>(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept {
  return lhs.index() <=> rhs.index();
}


} // namespace detail

template<entity Entity, allocator<Entity> Allocator>
inline typename basic_sparse_set<Entity, Allocator>::pointer basic_sparse_set<Entity, Allocator>::_sparse_page(const entity_type entity) const {
  const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
  const auto page = position / page_size;
  return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + fast_mod(position, page_size)) : nullptr;
}

template<entity Entity, allocator<Entity> Allocator>
inline typename basic_sparse_set<Entity, Allocator>::reference basic_sparse_set<Entity, Allocator>::_sparse_entry(const entity_type entity) const {
  SBX_ASSERT(_page(entity), "Invalid entity");
  const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
  return _sparse[position / page_size][fast_mod(position, page_size)];
}

template<entity Entity, allocator<Entity> Allocator>
inline typename basic_sparse_set<Entity, Allocator>::reference basic_sparse_set<Entity, Allocator>::_assure_page(const entity_type entity) {
  const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
  const auto page = position / page_size;

  if (page >= _sparse.size()) {
    _sparse.resize(page + size_type{1}, nullptr);
  }

  if (!_sparse[page]) {
    auto page_allocator = _packed.get_allocator();
    _sparse[page] = allocator_traits::allocate(page_allocator, page_size);
    std::uninitialized_fill(_sparse[page], _sparse[page] + page_size, nullptr);
  }

  auto& entry = _sparse[page][fast_mod(position, page_size)];
  SBX_ASSERT(entity_traits::to_version(entry) == entity_traits::to_version(tombstone_entity), "Slot unavailable");
  return entry;
}

template<entity Entity, allocator<Entity> Allocator>
void basic_sparse_set<Entity, Allocator>::_release_page_pages() {
  auto page_allocator = _packed.get_allocator();

  for (auto* page : _sparse) {
    if (page) {
      std::destroy(page, page + page_size);
      allocator_traits::deallocate(page_allocator, page, page_size);
      page = nullptr;
    }
  }
}

} // namespace sbx