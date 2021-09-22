#ifndef SBX_GFX_OBJECT_POOL_HPP_
#define SBX_GFX_OBJECT_POOL_HPP_

#include <vector>
#include <limits>
#include <utility>
#include <cassert>
#include <sstream>
#include <exception>

#include <types/primitives.hpp>

#include "index_traits.hpp"

namespace sbx {

template<typename Id, typename Value>
class sparse_set {

public:
  static_assert(is_index_type_v<Id>, "Id must be an index type");

  using id_type = Id;
  using value_type = Value;
  using iterator = typename std::vector<value_type>::iterator;
  using const_iterator = typename std::vector<value_type>::const_iterator;

  sparse_set() = default;
  ~sparse_set() = default;

  size_type size() const;

  void reserve(id_type size);

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

private:
  std::vector<id_type> _id_to_index;
  std::vector<id_type> _index_to_id;
  std::vector<value_type> _objects;
  std::vector<id_type> _free_ids;

}; // class sparse_set

template<typename Id, typename Value>
inline size_type sparse_set<Id, Value>::size() const {
  return _objects.size();
}

template<typename Id, typename Value>
inline void sparse_set<Id, Value>::reserve(id_type size) {
  _id_to_index.reserve(size);
  _index_to_id.reserve(size);
  _objects.reserve(size);
  _free_ids.reserve(size);
}

template<typename Id, typename Value>
inline typename sparse_set<Id, Value>::iterator sparse_set<Id, Value>::begin() {
  return _objects.begin();
}

template<typename Id, typename Value>
inline typename sparse_set<Id, Value>::const_iterator sparse_set<Id, Value>::begin() const {
  return _objects.begin();
}

template<typename Id, typename Value>
inline typename sparse_set<Id, Value>::iterator sparse_set<Id, Value>::end() {
  return _objects.end();
}

template<typename Id, typename Value>
inline typename sparse_set<Id, Value>::const_iterator sparse_set<Id, Value>::end() const {
  return _objects.end();
}

} // namespace sbx

#endif // SBX_GFX_OBJECT_POOL_HPP_
