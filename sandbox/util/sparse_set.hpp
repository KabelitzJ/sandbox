#ifndef SBX_GFX_OBJECT_POOL_HPP_
#define SBX_GFX_OBJECT_POOL_HPP_

#include <vector>
#include <limits>
#include <utility>
#include <cassert>
#include <sstream>
#include <exception>

#include <types/primitives.hpp>

namespace sbx {

template<typename Id, typename Value>
class sparse_set {

public:
  static_assert(std::is_integral_v<Id>, "id_type must be an integral type");

  using id_type = Id;
  using value_type = Value;
  using iterator = typename std::vector<value_type>::iterator;
  using const_iterator = typename std::vector<value_type>::const_iterator;

  sparse_set() = default;
  ~sparse_set() = default;

  template<typename... Args>
  std::pair<id_type, value_type&> emplace(Args&&... args);

  void remove(id_type id);

  bool has(id_type id) const;

  value_type& get(id_type id);
  const value_type& get(id_type id) const;

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
std::pair<typename sparse_set<Id, Value>::id_type, typename sparse_set<Id, Value>::value_type&> sparse_set<Id, Value>::emplace(Args&&... args) {
  const auto next_index = _objects.size();
  const auto& object = _objects.emplace_back(std::forward<Args>(args)...);

  auto id = INVALID_INDEX;

  if (!_free_ids.empty()) {
    id = _free_ids.back();
    _free_ids.pop_back();
    _id_to_index[id] = next_index;
  } else {
    id = _id_to_index.size();
    _id_to_index.push_back(next_index);
  }

  assert(id != INVALID_INDEX);

  _index_to_id.push_back(id);

  return std::make_pair(id, object);
}

template<typename Id, typename Value>
void sparse_set<Id, Value>::remove(id_type id) {
  const auto i = _id_to_index[id];

  std::swap(_objects[i], _objects.back());

  const auto last_object_id = _index_to_id.back();

  _id_to_index[last_object_id] = i;
  _index_to_id[i] = last_object_id;

  _objects.pop_back();
  _index_to_id.pop_back();

  _id_to_index[id] = INVALID_INDEX;

  _free_ids.push_back(id);
}

template<typename Id, typename Value>
inline bool sparse_set<Id, Value>::has(id_type id) const {
  return id < _id_to_index.size() && _id_to_index[id] != INVALID_INDEX;
}

template<typename Id, typename Value>
inline typename sparse_set<Id, Value>::value_type& sparse_set<Id, Value>::get(id_type id){
  if (id >= _id_to_index.size()) {
    throw std::invalid_argument();
  }

  return _objects[_id_to_index[id]];
}

template<typename Id, typename Value>
inline const typename sparse_set<Id, Value>::value_type& sparse_set<Id, Value>::get(id_type id) const {
  return get(id);
}

template<typename Id, typename Value>
inline typename sparse_set<Id, Value>::value_type& sparse_set<Id, Value>::operator[](id_type id) {
  return get(id);
}

template<typename Id, typename Value>
inline const typename sparse_set<Id, Value>::value_type& sparse_set<Id, Value>::operator[](id_type id) const {
  return get(id);
}

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
