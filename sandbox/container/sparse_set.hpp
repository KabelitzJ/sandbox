#ifndef SBX_CONTAINER_SPARSE_SET_HPP_
#define SBX_CONTAINER_SPARSE_SET_HPP_

#include <concepts>
#include <memory>
#include <vector>
#include <limits>

#include <math/functional.hpp>

#include <meta/concepts.hpp>

namespace sbx {

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
requires (std::has_single_bit(PageSize))
class basic_sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;

  using sparse_container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<typename allocator_traits::pointer>>;
  using dense_container_type = std::vector<Type, Allocator>;

  inline static constexpr auto page_size = PageSize;
  inline static constexpr auto placeholder = std::numeric_limits<Type>::max();

public:

  using allocator_type = typename allocator_traits::allocator_type;
  using value_type = typename allocator_traits::value_type;
  using const_pointer = typename dense_container_type::const_pointer;
  using const_reference = typename dense_container_type::const_reference;
  using size_type = typename dense_container_type::size_type;
  using iterator = typename dense_container_type::iterator;


  basic_sparse_set();

  explicit basic_sparse_set(const allocator_type& allocator);

  basic_sparse_set(const basic_sparse_set& other) = delete;

  basic_sparse_set(basic_sparse_set&& other) noexcept;

  basic_sparse_set& operator=(const basic_sparse_set& other) = delete;

  basic_sparse_set& operator=(basic_sparse_set&& other) noexcept;

  virtual ~basic_sparse_set();

  bool contains(const value_type value) const noexcept;

protected:

private:

  using reference = typename dense_container_type::reference;
  using pointer = typename dense_container_type::pointer;

  reference _get_sparse_element(const value_type value);

  void _release_sparse_page();

  sparse_container_type _sparse{};
  dense_container_type _dense{};

}; // class sparse_set

template<std::unsigned_integral Type>
using sparse_set = basic_sparse_set<Type, std::allocator<Type>, std::size_t{4096}>;

} // namespace sbx

#include "sparse_set.inl"

#endif // SBX_CONTAINER_SPARSE_SET_HPP_
