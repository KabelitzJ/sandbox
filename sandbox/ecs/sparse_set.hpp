#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <vector>
#include <unordered_map>

#include "entity.hpp"

namespace sbx {

class sparse_set {

  using sparse_container_type = std::unordered_map<entity::value_type, std::size_t>;
  using dense_container_type = std::vector<entity>;

public:

  using value_type = entity;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using const_reference = const value_type&;
  using const_pointer = const value_type*;
  using const_iterator = dense_container_type::const_iterator;

  sparse_set() = default;

  sparse_set(const sparse_set& other) = delete;

  sparse_set(sparse_set&& other) noexcept;

  virtual ~sparse_set() = default;

  sparse_set& operator=(const sparse_set& other) = delete;

  sparse_set& operator=(sparse_set&& other) noexcept;

private:

  sparse_container_type _sparse{};
  dense_container_type _dense{};

}; // class sparse_set

} // namespace sbx

#endif // SBX_ECS_SPARSE_SET_HPP_
