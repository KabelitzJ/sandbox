#ifndef SBX_GFX_OBJECT_POOL_HPP_
#define SBX_GFX_OBJECT_POOL_HPP_

#include <vector>
#include <limits>

#include <types/primitives.hpp>

namespace sbx {

template<typename IndexType, typename ValueType>
class sparse_set {

public:
  static_assert(std::is_integral_v<IndexType>, "IndexType must be an integral type");

  // (TODO) KAJ 19.09.2021: Find solution for name conflict index_type. What are the alternatives??
  using index_type = IndexType;
  using value_type = ValueType; 

  sparse_set() = default;
  ~sparse_set() = default;

  size_type size() const;

  void reserve(index_type size);

private:
  std::vector<index_type> _id_to_index;
  std::vector<index_type> _free_ids;
  std::vector<value_type> _objects;
  std::vector<index_type> _index_to_id;

}; // class sparse_set


template<typename Id, typename T>
inline size_type sparse_set<Id, T>::size() const {
  return _objects.size();
}

template<typename Id, typename T>
inline void sparse_set<Id, T>::reserve(index_type size) {
  _id_to_index.reserve(size);
  _free_ids.reserve(size);
  _objects.reserve(size);
  _index_to_id.reserve(size);
}

} // namespace sbx

#endif // SBX_GFX_OBJECT_POOL_HPP_
