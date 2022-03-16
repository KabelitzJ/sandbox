#include "entity_set.hpp"

#include <math/functional.hpp>

#include <platform/assert.hpp>

namespace sbx {

namespace detail {

entity_set_iterator::entity_set_iterator(const container_type& data, const size_type offset) noexcept
: _data{std::addressof(data)},
  _offset{offset} { }

entity_set_iterator& entity_set_iterator::operator++() noexcept {
  --_offset;
  return *this;
}

entity_set_iterator entity_set_iterator::operator++(int) noexcept {
  const auto copy = entity_set_iterator{*this};
  --_offset;
  return copy;
}

entity_set_iterator& entity_set_iterator::operator--() noexcept {
  ++_offset;
  return *this;
}

entity_set_iterator entity_set_iterator::operator--(int) noexcept {
  const auto copy = entity_set_iterator{*this};
  ++_offset;
  return copy;
}

entity_set_iterator& entity_set_iterator::operator+=(const difference_type offset) noexcept {
  _offset -= static_cast<size_type>(offset);
  return *this;
}

entity_set_iterator entity_set_iterator::operator+(const difference_type offset) const noexcept {
  auto copy = entity_set_iterator{*this};
  return (copy += offset);
}

entity_set_iterator& entity_set_iterator::operator-=(const difference_type offset) noexcept {
  _offset += static_cast<size_type>(offset);
  return *this;
}

entity_set_iterator entity_set_iterator::operator-(const difference_type offset) const noexcept {
  auto copy = entity_set_iterator{*this};
  return (copy -= offset);
}

entity_set_iterator::const_reference entity_set_iterator::operator*() const noexcept {
  return _data->at(index());
}

entity_set_iterator::const_pointer entity_set_iterator::operator->() const noexcept {
  return std::addressof(_data->at(index()));
} 

entity_set_iterator::size_type entity_set_iterator::index() const noexcept {
  return _offset - size_type{1};
}

} // namespace detail

entity_set::size_type entity_set::size() const noexcept {
  return _dense.size();
}

bool entity_set::empty() const noexcept {
  return _dense.empty();
}

entity_set::const_iterator entity_set::begin() const noexcept {
  return const_iterator{_dense, _dense.size()};
}

entity_set::const_iterator entity_set::cbegin() const noexcept {
  return begin();
}

entity_set::const_iterator entity_set::end() const noexcept {
  return const_iterator{_dense, size_type{0}};
}

entity_set::const_iterator entity_set::cend() const noexcept {
  return end();
}

entity_set::const_reverse_iterator entity_set::rbegin() const noexcept {
  return std::make_reverse_iterator(end());
}

entity_set::const_reverse_iterator entity_set::crbegin() const noexcept {
  return rbegin();
}

entity_set::const_reverse_iterator entity_set::rend() const noexcept {
  return std::make_reverse_iterator(begin());
}

entity_set::const_reverse_iterator entity_set::crend() const noexcept {
  return rend();
}

entity_set::const_iterator entity_set::emplace(const entity& entity) {
  // return _try_emplace(entity);
}

void entity_set::erase(const entity& e) {
  // const auto itr = --(end() - static_cast<difference_type>(index(e)));

  // _swap_and_pop(itr, itr + difference_type{1});
}

entity_set::const_iterator entity_set::find(const entity& entity) const noexcept {
  // return contains(entity) ? --(end() - static_cast<difference_type>(index(entity))) : end();
}

bool entity_set::contains(const entity& entity) const noexcept {
  // const auto index = static_cast<size_type>(entity._id());
  // const auto page = index / sparse_page_size;

  // if (page >= _sparse.size()) {
  //   return false;
  // }

  // const auto& sparse_element = _sparse[page][fast_mod(index, sparse_page_size)];

  // if (sparse_element == entity::null._id()) {
  //   return false;
  // }

  // return _dense[static_cast<size_type>(sparse_element)] == entity;
}

entity_set::size_type entity_set::index(const entity& entity) const {
  // if (!contains(entity)) {
  //   throw std::out_of_range{"entity not found"};
  // }

  // return static_cast<size_type>(_sparse_element(entity)); 
}

entity_set::const_iterator entity_set::_try_emplace(const entity& entity) {
  // _assure_spare_element(entity) = static_cast<entity::id_type>(_dense.size());
  // _dense.push_back(entity); 

  // return cbegin();
}

void entity_set::_swap_and_pop(const_iterator first, const_iterator last) {
  // for (; first != last; ++first) {
  //   _sparse_element(_dense.back()) = static_cast<entity::id_type>(first.index());

  //   const auto entity = std::exchange(_dense[first.index()], _dense.back());

  //   _sparse_element(entity) = entity::null._id();

  //   _dense.pop_back();
  // }
}

} // namespace sbx
