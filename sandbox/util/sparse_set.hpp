#ifndef SBX_GFX_OBJECT_POOL_HPP_
#define SBX_GFX_OBJECT_POOL_HPP_

#include <vector>
#include <limits>

namespace sbx {

template<typename IndexType, typename T>
class sparse_set {

public:
  static_assert(std::is_integral_v<IndexType>, "IndexType must be an integral type");

  static constexpr auto INVALID_INDEX = std::numeric_limits<IndexType>::max();

  using index_type = IndexType;
  using type = T; 

  sparse_set() = default;
  ~sparse_set() = default;

  std::size_t size() const;

  void reserve(std::size_t size);

private:
  std::vector<std::size_t> _id_to_index;
  std::vector<index_type> _free_ids;
  std::vector<type> _objects;
  std::vector<index_type> _index_to_id;

}; // class sparse_set


template<typename Id, typename T>
inline std::size_t sparse_set<Id, T>::size() const {
  return _objects.size();
}

template<typename Id, typename T>
inline void sparse_set<Id, T>::reserve(std::size_t size) {
  _id_to_index.reserve(size);
  _free_ids.reserve(size);
  _objects.reserve(size);
  _index_to_id.reserve(size);
}

} // namespace sbx

#endif // SBX_GFX_OBJECT_POOL_HPP_
