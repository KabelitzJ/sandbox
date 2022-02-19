#include <math/functional.hpp>

#include <platform/assert.hpp>

namespace sbx {

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline basic_sparse_set<Type, Allocator, PageSize>::basic_sparse_set()
: _sparse{}, 
  _packed{}, 
  _free_list{placeholder} { }

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline basic_sparse_set<Type, Allocator, PageSize>::basic_sparse_set(const allocator_type& allocator)
: _sparse{allocator}, 
  _packed{allocator}, 
  _free_list{placeholder} { }

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline basic_sparse_set<Type, Allocator, PageSize>::basic_sparse_set(const basic_sparse_set<Type, Allocator, PageSize>&& other) noexcept
: _sparse{std::move(other._sparse)}, 
  _packed{std::move(other._packed)}, 
  _free_list{std::exchange(other._free_list, placeholder)} { }

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline basic_sparse_set<Type, Allocator, PageSize>::~basic_sparse_set() {
  _release_sparse_pages();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline basic_sparse_set<Type, Allocator, PageSize>& basic_sparse_set<Type, Allocator, PageSize>::operator=(const basic_sparse_set<Type, Allocator, PageSize>&& other) noexcept {
  _release_sparse_pages();
  
  _sparse = std::move(other._sparse);
  _packed = std::move(other._packed);
  _free_list = std::exchange(other._free_list, placeholder);

  return *this;
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline constexpr typename basic_sparse_set<Type, Allocator, PageSize>::allocator_type basic_sparse_set<Type, Allocator, PageSize>::get_allocator() const noexcept {
  return _packed.get_allocator();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline typename basic_sparse_set<Type, Allocator, PageSize>::size_type basic_sparse_set<Type, Allocator, PageSize>::size() const noexcept {
  return _packed.size();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline bool basic_sparse_set<Type, Allocator, PageSize>::empty() const noexcept {
  return _packed.empty();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline typename basic_sparse_set<Type, Allocator, PageSize>::size_type basic_sparse_set<Type, Allocator, PageSize>::capacity() const noexcept {
  return _packed.capacity();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
void basic_sparse_set<Type, Allocator, PageSize>::reserve(const size_type capacity) {
  _packed.reserve(capacity);
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
void basic_sparse_set<Type, Allocator, PageSize>::shrink_to_fit() {
  _packed.shrink_to_fit();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_pointer basic_sparse_set<Type, Allocator, PageSize>::data() const noexcept {
  return _packed.data();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_iterator basic_sparse_set<Type, Allocator, PageSize>::begin() const noexcept {
  return _packed.begin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_iterator basic_sparse_set<Type, Allocator, PageSize>::cbegin() const noexcept {
  return _packed.cbegin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_iterator basic_sparse_set<Type, Allocator, PageSize>::end() const noexcept {
  return _packed.end();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_iterator basic_sparse_set<Type, Allocator, PageSize>::cend() const noexcept {
  return _packed.cend();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_reverse_iterator basic_sparse_set<Type, Allocator, PageSize>::rbegin() const noexcept {
  return _packed.rbegin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_reverse_iterator basic_sparse_set<Type, Allocator, PageSize>::crbegin() const noexcept {
  return _packed.crbegin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_reverse_iterator basic_sparse_set<Type, Allocator, PageSize>::rend() const noexcept {
  return _packed.rend();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_reverse_iterator basic_sparse_set<Type, Allocator, PageSize>::crend() const noexcept {
  return _packed.crend();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::size_type basic_sparse_set<Type, Allocator, PageSize>::index(const value_type value) const noexcept {
  SBX_ASSERT(contains(value), "Set does not contain value");
  return static_cast<size_type>(_sparse_element(value));
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
bool basic_sparse_set<Type, Allocator, PageSize>::contains(const value_type value) const noexcept {
  const auto element = _sparse_page(value);
  return element && (((~placeholder & value) ^ *element) < placeholder);
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_iterator basic_sparse_set<Type, Allocator, PageSize>::find(const value_type value) const noexcept {
  return contains(value) ? --(end() - static_cast<difference_type>(index(value))) : end();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::value_type basic_sparse_set<Type, Allocator, PageSize>::at(const size_type index) const {
  return index < _packed.size() ? _packed[index] : placeholder;
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::value_type basic_sparse_set<Type, Allocator, PageSize>::operator[](const size_type index) const {
  SBX_ASSERT(index < _packed.size(), "Index out of range");
  return _packed[index];
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_iterator basic_sparse_set<Type, Allocator, PageSize>::insert(const value_type value) {
  return contains(value) ? end() : _try_insert(value);
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
void basic_sparse_set<Type, Allocator, PageSize>::remove(const value_type value) {
  if (contains(value)) {
    _swap_and_pop(value);
  }
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::const_iterator basic_sparse_set<Type, Allocator, PageSize>::_try_insert(const value_type value) {
  SBX_ASSERT(!contains(value), "Set already contains value");

  if (auto& element = _assure_page(value); _free_list == placeholder) {
    _packed.push_back(value);
    element = static_cast<value_type>(_packed.size() - size_type{1});
    return end();
  } else {
    const auto position = static_cast<size_type>(value);
    element = _free_list;
    _free_list = std::exchange(_packed[position], value);
    return --(end() - static_cast<difference_type>(element));
  }
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
void basic_sparse_set<Type, Allocator, PageSize>::_swap_and_pop(const value_type value) {
  SBX_ASSERT(contains(value), "Set does not contain value");

  _sparse_element(_packed.back()) = value;
  const auto other = std::exchange(_packed[index(value)], _packed.back());

  SBX_ASSERT((_packed.back() = placeholder, true), "");

  _sparse_element(other) = placeholder;
  _packed.pop_back();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::pointer basic_sparse_set<Type, Allocator, PageSize>::_sparse_page(const value_type value) const {
  const auto position = static_cast<size_type>(value);
  const auto page = position / page_size;
  return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + fast_mod(position, page_size)) : nullptr;
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::reference basic_sparse_set<Type, Allocator, PageSize>::_sparse_element(const value_type value) const {
  SBX_ASSERT(_sparse_page(value), "Invalid element");
  const auto position = static_cast<size_type>(value);
  return _sparse[position / page_size][fast_mod(position, page_size)];
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::reference basic_sparse_set<Type, Allocator, PageSize>::_assure_page(const value_type value) {
  const auto position = static_cast<size_type>(value);
  const auto page = position / page_size;

  if (page >= _sparse.size()) {
    _sparse.resize(page + size_type{1}, nullptr);
  }

  if (!_sparse[page]) {
    auto page_allocator = allocator_type{_packed.get_allocator()};
    _sparse[page] = allocator_traits::allocate(page_allocator, page_size);
    std::uninitialized_fill(_sparse[page], _sparse[page] + page_size, placeholder);
  }

  auto& element = _sparse[page][fast_mod(position, page_size)];

  SBX_ASSERT(element == placeholder, "Element already exists");

  return element;
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
inline void basic_sparse_set<Type, Allocator, PageSize>::_release_sparse_pages() {
  auto page_allocator = allocator_type{_packed.get_allocator()};

  for (auto* page : _sparse) {
    if (page) {
      std::destroy(page, page + page_size);
      allocator_traits::deallocate(page_allocator, page, page_size);
      page = nullptr;
    }
  }
}

} // namespace sbx