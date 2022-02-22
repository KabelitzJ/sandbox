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

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_reverse_iterator basic_sparse_set<Type, Allocator>::rbegin() const noexcept {
  return _dense.crbegin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_reverse_iterator basic_sparse_set<Type, Allocator>::crbegin() const noexcept {
  return _dense.crbegin();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_reverse_iterator basic_sparse_set<Type, Allocator>::rend() const noexcept {
  return _dense.crend();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_reverse_iterator basic_sparse_set<Type, Allocator>::crend() const noexcept {
  return _dense.crend();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::size_type basic_sparse_set<Type, Allocator>::size() const noexcept {
  return _dense.size();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
bool basic_sparse_set<Type, Allocator>::empty() const noexcept {
  return _dense.empty();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_iterator basic_sparse_set<Type, Allocator>::find(const value_type& value) const noexcept {
  return contains(value) ? _dense.cbegin() + _sparse.at(value) : _dense.cend();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
bool basic_sparse_set<Type, Allocator>::contains(const value_type& value) const noexcept {
  const auto index = static_cast<size_type>(value);
  return index < _sparse.size() && _sparse[index] < _dense.size() && _dense[_sparse[index]] == value;
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_iterator basic_sparse_set<Type, Allocator>::insert(const value_type& value) {
  if (const auto& entry = find(value); entry != cend()) {
    return entry;
  }

  const auto index = static_cast<size_type>(value);

  if (index >= _sparse.size()) {
    _sparse.resize(index + size_type{1});
  }

  _sparse[index] = static_cast<value_type>(_dense.size());
  _dense.push_back(value);

  return std::prev(_dense.cend());
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
void basic_sparse_set<Type, Allocator>::erase(const value_type& value) {
  if (!contains(value)) {
    return;
  }

  const auto index = static_cast<size_type>(value);
  const auto last = _dense.back();

  _dense[_sparse[index]] = last;
  _sparse[last] = _sparse[index];
  _dense.pop_back();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
void basic_sparse_set<Type, Allocator>::clear() noexcept {
  _dense.clear();
  _sparse.clear();
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_reference basic_sparse_set<Type, Allocator>::operator[](const size_type& index) const noexcept {
  return _dense[index];
}

template<std::unsigned_integral Type, allocator<Type> Allocator>
basic_sparse_set<Type, Allocator>::const_reference basic_sparse_set<Type, Allocator>::at(const size_type& index) const {
  return _dense.at(index);
}

} // namespace sbx