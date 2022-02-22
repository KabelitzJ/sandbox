#ifndef SBX_CONTAINER_SPARSE_SET_HPP_
#define SBX_CONTAINER_SPARSE_SET_HPP_

#include <concepts>
#include <vector>
#include <memory>

#include <meta/concepts.hpp>

namespace sbx {

template<std::unsigned_integral Type, allocator<Type> Allocator>
class basic_sparse_set {

  using sparse_container_type = std::vector<Type, Allocator>;
  using dense_container_type = std::vector<Type, Allocator>;

public:

  using value_type = Type;
  using allocator_type = Allocator;
  using size_type = dense_container_type::size_type;
  using difference_type = dense_container_type::difference_type;
  using const_reference = dense_container_type::const_reference;
  using const_pointer = dense_container_type::const_pointer;
  using const_iterator = dense_container_type::const_iterator;

  basic_sparse_set() = default;

  explicit basic_sparse_set(const allocator_type& allocator);

  basic_sparse_set(const basic_sparse_set& other) = delete;

  basic_sparse_set(basic_sparse_set&& other) noexcept;

  virtual ~basic_sparse_set() = default;

  basic_sparse_set& operator=(const basic_sparse_set& other) = delete;

  basic_sparse_set& operator=(basic_sparse_set&& other) noexcept;

  const_iterator begin() const noexcept;

  const_iterator cbegin() const noexcept;

  const_iterator end() const noexcept;

  const_iterator cend() const noexcept;

private:

  sparse_container_type _sparse{};
  dense_container_type _dense{};

}; // class basic_sparse_set

template<std::unsigned_integral Type>
using sparse_set = basic_sparse_set<Type, std::allocator<Type>>; 

} // namespace sbx

#include "sparse_set.inl"

#endif // SBX_CONTAINER_SPARSE_SET_HPP_
