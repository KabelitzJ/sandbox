#include <math/functional.hpp>

#include <platform/assert.hpp>

namespace sbx {

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
basic_sparse_set<Type, Allocator, PageSize>::basic_sparse_set()
: _sparse{},
  _dense{} { }

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
basic_sparse_set<Type, Allocator, PageSize>::basic_sparse_set(const allocator_type& allocator)
: _sparse{allocator},
  _dense{allocator} { }

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
basic_sparse_set<Type, Allocator, PageSize>::basic_sparse_set(basic_sparse_set<Type, Allocator, PageSize>&& other) noexcept
: _sparse{std::move(other._sparse)},
  _dense{std::move(other._dense)} { }

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
basic_sparse_set<Type, Allocator, PageSize>::~basic_sparse_set() {
  _release_sparse_page();
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
basic_sparse_set<Type, Allocator, PageSize>& basic_sparse_set<Type, Allocator, PageSize>::operator=(basic_sparse_set<Type, Allocator, PageSize>&& other) noexcept {
  _sparse = std::move(other._sparse);
  _dense = std::move(other._dense);
  return *this;
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
bool basic_sparse_set<Type, Allocator, PageSize>::contains(const value_type value) const noexcept {
  return false;
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
typename basic_sparse_set<Type, Allocator, PageSize>::reference basic_sparse_set<Type, Allocator, PageSize>::_get_sparse_element(const value_type value) {
  const auto position = static_cast<size_type>(value);
  const auto page = position / page_size;

  if (page >= _sparse.size()) {
    _sparse.resize(page + size_type{1}, nullptr);
  }

  if (!_sparse[page]) {
    auto page_allocator = allocator_type{_dense.get_allocator()};
    _sparse[page] = allocator_traits::allocate(page_allocator, page_size);
    std::uninitialized_fill_n(_sparse[page], page_size, value_type{});
  }

  return _sparse[page][fast_mod<value_type, page_size>()];
}

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
void basic_sparse_set<Type, Allocator, PageSize>::_release_sparse_page() {
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