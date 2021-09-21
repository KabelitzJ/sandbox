#ifndef SBX_GFX_OBJECT_POOL_HPP_
#define SBX_GFX_OBJECT_POOL_HPP_

#include <vector>
#include <limits>
#include <utility>
#include <cassert>
#include <sstream>
#include <exception>

#include <types/primitives.hpp>
#include <types/basic_traits.hpp>

namespace sbx {

template<typename Id, typename Value>
class sparse_set {

public:
  static_assert(is_index_v<Id>, "id_type must be an index type");

  using id_type = Id;
  using value_type = Value;
  using iterator = typename std::vector<value_type>::iterator;
  using const_iterator = typename std::vector<value_type>::const_iterator;

  sparse_set() = default;
  ~sparse_set() = default;

  template<typename... Args>
  value_type& emplace(id_type id, Args&&... args);

  void remove(id_type id);

  bool has(id_type id) const;

  value_type& at(id_type id);
  const value_type& at(id_type id) const;

  value_type& operator[](id_type id);
  const value_type& operator[](id_type id) const;

  size_type size() const;

  void reserve(id_type size);

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

private:
  static constexpr auto INVALID_INDEX = std::numeric_limits<id_type>::max();

  std::vector<id_type> _id_to_index;
  std::vector<id_type> _index_to_id;
  std::vector<value_type> _objects;
  std::vector<id_type> _free_ids;

}; // class sparse_set


template<typename Id, typename Value>
template<typename... Args>
inline auto sparse_set<Id, Value>::emplace(id_type id, Args&&... args) -> value_type& {
  
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::remove(id_type id) -> void {
  
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::has(id_type id) const -> bool {
  return id < _id_to_index.size() && _id_to_index[id] != INVALID_INDEX;
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::at(id_type id) -> value_type& {
  return _objects[_id_to_index.at(id)];
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::at(id_type id) const -> const value_type& {
  return at(id);
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::operator[](id_type id) -> value_type& {
  return at(id);
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::operator[](id_type id) const -> const value_type& {
  return at(id);
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::size() const -> size_type {
  return _objects.size();
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::reserve(id_type size) -> void {
  _id_to_index.reserve(size);
  _index_to_id.reserve(size);
  _objects.reserve(size);
  _free_ids.reserve(size);
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::begin() -> iterator {
  return _objects.begin();
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::begin() const -> const_iterator {
  return _objects.begin();
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::end() -> iterator {
  return _objects.end();
}

template<typename Id, typename Value>
inline auto sparse_set<Id, Value>::end() const -> const_iterator {
  return _objects.end();
}

} // namespace sbx

#endif // SBX_GFX_OBJECT_POOL_HPP_
