#ifndef SBX_CONTAINER_SPARSE_SET_HPP_
#define SBX_CONTAINER_SPARSE_SET_HPP_

#include <concepts>
#include <limits>
#include <memory>
#include <vector>

#include <meta/concepts.hpp>

namespace sbx {

namespace detail {

template<container Container>
struct sparse_set_iterator {

  using container_type = Container;
  using value_type = container_type::value_type;
  using size_type = container_type::size_type;
  using difference_type = container_type::difference_type;
  using reference = container_type::reference;
  using pointer = container_type::pointer;
  using iterator_category = std::bidirectional_iterator_tag;

  sparse_set_iterator(const container_type& container, const difference_type offset) noexcept;

  ~sparse_set_iterator() = default;

  sparse_set_iterator& operator++() noexcept;

  sparse_set_iterator operator++(int) noexcept;

  sparse_set_iterator& operator--() noexcept;

  sparse_set_iterator operator--(int) noexcept;

  sparse_set_iterator& operator+=(const difference_type offset) noexcept;

  sparse_set_iterator operator+(const difference_type offset) const noexcept;

  sparse_set_iterator& operator-=(const difference_type offset) noexcept;

  sparse_set_iterator operator-(const difference_type offset) const noexcept;

  [[nodiscard]] reference operator*() const;

  [[nodiscard]] pointer operator->() const;

  [[nodiscard]] size_type index() const noexcept;

private:

  const container_type* _container{};
  difference_type _offset{};

}; // struct sparse_set_iterator

template<container Container>
bool operator==(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept;

template<container Container>
std::strong_ordering operator<=>(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept;

} // namespace detail

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
class basic_sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;

  using sparse_container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<typename allocator_traits::pointer>>;
  using dense_container_type = std::vector<Type, Allocator>;

  using reference = dense_container_type::reference;
  using pointer = dense_container_type::pointer;

  inline static constexpr auto page_size = PageSize;
  inline static constexpr auto placeholder = std::numeric_limits<Type>::max();

public:

  using value_type = Type;
  using allocator_type = Allocator;
  using size_type = dense_container_type::size_type;
  using difference_type = dense_container_type::difference_type;
  using const_reference = dense_container_type::const_reference;
  using const_pointer = dense_container_type::const_pointer;
  using const_iterator = detail::sparse_set_iterator<dense_container_type>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  basic_sparse_set();

  explicit basic_sparse_set(const allocator_type& allocator);

  basic_sparse_set(const basic_sparse_set&) = delete;

  basic_sparse_set(basic_sparse_set&&) noexcept;

  virtual ~basic_sparse_set();

  basic_sparse_set& operator=(const basic_sparse_set&) = delete;

  basic_sparse_set& operator=(basic_sparse_set&&) noexcept;

  [[nodiscard]] size_type size() const noexcept;

  [[nodiscard]] bool empty() const noexcept;

  [[nodiscard]] const_pointer data() const noexcept;

  [[nodiscard]] const_iterator begin() const noexcept;

  [[nodiscard]] const_iterator cbegin() const noexcept;

  [[nodiscard]] const_iterator end() const noexcept;

  [[nodiscard]] const_iterator cend() const noexcept;

  [[nodiscard]] const_iterator find(const Type value) const noexcept;

  [[nodiscard]] bool contains(const value_type value) const noexcept;

  [[nodiscard]] size_type index(const value_type value) const noexcept;

  [[nodiscard]] const_reference at(const size_type index) const;

  [[nodiscard]] const_reference operator[](const size_type index) const noexcept;

  const_iterator insert(const value_type value);

  void remove(const value_type value);

protected:

  virtual void _swap_and_pop(const_iterator first, const_iterator last);

  virtual const_iterator _try_insert(const value_type value);

private:

  [[nodiscard]] pointer _sparse_pointer(const value_type value) const;

  [[nodiscard]] reference _sparse_reference(const value_type value) const;

  [[nodiscard]] reference _assure_at_least(const value_type value);

  void _release_sparse_pages();

  sparse_container_type _sparse{};
  dense_container_type _dense{};
  value_type _free_list{};

}; // class basic_sparse_set

template<std::unsigned_integral Type, std::size_t PageSize>
using sparse_set = basic_sparse_set<Type, std::allocator<Type>, PageSize>;

} // namespace s

#include "sparse_set.inl"

#endif // SBX_CONTAINER_SPARSE_SET_HPP_
