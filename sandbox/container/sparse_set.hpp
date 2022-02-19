#ifndef SBX_CONTAINER_SPARSE_SET_HPP_
#define SBX_CONTAINER_SPARSE_SET_HPP_

#include <concepts>
#include <memory>
#include <vector>
#include <bit>
#include <limits>

#include <meta/concepts.hpp>

#include <math/functional.hpp>

namespace sbx {

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
requires(std::has_single_bit(PageSize))
class basic_sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;

  using sparse_container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<typename allocator_traits::pointer>>;
  using packed_container_type = std::vector<typename allocator_traits::value_type, typename allocator_traits::allocator_type>;

  inline static constexpr auto page_size = PageSize;
  inline static constexpr auto placeholder = std::numeric_limits<Type>::max();

public:

  using allocator_type = typename packed_container_type::allocator_type;
  using value_type = typename packed_container_type::value_type;
  using reference = typename packed_container_type::reference;
  using const_reference = typename packed_container_type::const_reference;
  using pointer = typename packed_container_type::pointer;
  using const_pointer = typename packed_container_type::const_pointer;
  using size_type = typename packed_container_type::size_type;
  using difference_type = typename packed_container_type::difference_type;
  using const_iterator = typename packed_container_type::const_iterator;
  using const_reverse_iterator = typename packed_container_type::const_reverse_iterator;

  basic_sparse_set();

  explicit basic_sparse_set(const allocator_type& allocator);

  basic_sparse_set(const basic_sparse_set<Type, Allocator, PageSize>& other) = delete;

  basic_sparse_set(const basic_sparse_set<Type, Allocator, PageSize>&& other) noexcept;

  virtual ~basic_sparse_set();

  basic_sparse_set<Type, Allocator, PageSize>& operator=(const basic_sparse_set<Type, Allocator, PageSize>& other) = delete;

  basic_sparse_set<Type, Allocator, PageSize>& operator=(const basic_sparse_set<Type, Allocator, PageSize>&& other) noexcept;

  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept;

  [[nodiscard]] size_type size() const noexcept;

  [[nodiscard]] bool empty() const noexcept;

  [[nodiscard]] virtual size_type capacity() const noexcept;

  virtual void reserve(const size_type capacity);

  virtual void shrink_to_fit();

  [[nodiscard]] const_pointer data() const noexcept;

  [[nodiscard]] const_iterator begin() const noexcept;

  [[nodiscard]] const_iterator cbegin() const noexcept;

  [[nodiscard]] const_iterator end() const noexcept;

  [[nodiscard]] const_iterator cend() const noexcept;

  [[nodiscard]] const_reverse_iterator rbegin() const noexcept;

  [[nodiscard]] const_reverse_iterator crbegin() const noexcept;

  [[nodiscard]] const_reverse_iterator rend() const noexcept;

  [[nodiscard]] const_reverse_iterator crend() const noexcept;

  [[nodiscard]] size_type index(const value_type value) const noexcept;

  [[nodiscard]] bool contains(const value_type value) const noexcept;

  [[nodiscard]] const_iterator find(const value_type value) const noexcept;

  [[nodiscard]] value_type at(const size_type index) const;

  [[nodiscard]] value_type operator[](const size_type index) const;

  const_iterator insert(const value_type value);

  void remove(const value_type value);

protected:

  virtual const_iterator _try_insert(const value_type value);

  virtual void _swap_and_pop(const value_type value);

private:

  [[nodiscard]] pointer _sparse_page(const value_type value) const;

  [[nodiscard]] reference _sparse_element(const value_type value) const;

  [[nodiscard]] reference _assure_page(const value_type value);

  void _release_sparse_pages();

  sparse_container_type _sparse{};
  packed_container_type _packed{};
  value_type _free_list{};

}; // class basic_sparse_set

template<std::unsigned_integral Type>
using sparse_set = basic_sparse_set<Type, std::allocator<Type>, std::size_t{4096}>;

} // namespace sbx

#include "sparse_set.inl"

#endif // SBX_CONTAINER_SPARSE_SET_HPP_
