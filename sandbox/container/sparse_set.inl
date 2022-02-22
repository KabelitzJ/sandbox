#include <utility>

namespace sbx {

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::basic_sparse_set(const allocator_type& allocator)
: _sparse{allocator},
  _dense{allocator} { }

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::basic_sparse_set(basic_sparse_set&& other) noexcept
: _sparse{std::move(other._sparse)},
  _dense{std::move(other._dense)} { }

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>& basic_sparse_set<Type, Allocator>::operator=(basic_sparse_set&& other) noexcept {
  _sparse = std::move(other._sparse);
  _dense = std::move(other._dense);

  return *this;
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_iterator basic_sparse_set<Type, Allocator>::begin() const noexcept {
  return _dense.cbegin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_iterator basic_sparse_set<Type, Allocator>::cbegin() const noexcept {
  return _dense.cbegin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_iterator basic_sparse_set<Type, Allocator>::end() const noexcept {
  return _dense.cend();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_iterator basic_sparse_set<Type, Allocator>::cend() const noexcept {
  return _dense.cend();
}

} // namespace sbx