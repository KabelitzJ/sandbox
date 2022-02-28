#ifndef SBX_ECS_ENTITY_SET_HPP_
#define SBX_ECS_ENTITY_SET_HPP_

#include <concepts>
#include <limits>
#include <memory>
#include <vector>

#include <meta/concepts.hpp>

#include "entity_traits.hpp"

namespace sbx {

namespace detail {

template<container Container>
struct entity_set_iterator {

  using container_type = Container;
  using value_type = container_type::value_type;
  using size_type = container_type::size_type;
  using difference_type = container_type::difference_type;
  using reference = container_type::reference;
  using pointer = container_type::pointer;
  using iterator_category = std::bidirectional_iterator_tag;

  entity_set_iterator(const container_type& container, const difference_type offset) noexcept;

  ~entity_set_iterator() = default;

  entity_set_iterator& operator++() noexcept;

  entity_set_iterator operator++(int) noexcept;

  entity_set_iterator& operator--() noexcept;

  entity_set_iterator operator--(int) noexcept;

  entity_set_iterator& operator+=(const difference_type offset) noexcept;

  entity_set_iterator operator+(const difference_type offset) const noexcept;

  entity_set_iterator& operator-=(const difference_type offset) noexcept;

  entity_set_iterator operator-(const difference_type offset) const noexcept;

  [[nodiscard]] reference operator*() const;

  [[nodiscard]] pointer operator->() const;

  [[nodiscard]] size_type index() const noexcept;

private:

  const container_type* _container{};
  difference_type _offset{};

}; // struct entity_set_iterator

template<container Container>
bool operator==(const entity_set_iterator<Container>& lhs, const entity_set_iterator<Container>& rhs) noexcept;

template<container Container>
std::strong_ordering operator<=>(const entity_set_iterator<Container>& lhs, const entity_set_iterator<Container>& rhs) noexcept;

} // namespace detail

template<entity Entity, allocator<Entity> Allocator>
class basic_entity_set {

  using allocator_traits = std::allocator_traits<Allocator>;
  using entity_traits = entity_traits<Entity>;

  using sparse_container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<typename allocator_traits::pointer>>;
  using dense_container_type = std::vector<Entity, Allocator>;

  using reference = dense_container_type::reference;
  using pointer = dense_container_type::pointer;

  inline static constexpr auto page_size = std::size_t{4096};

public:

  using entity_type = Entity;
  using version_type = entity_traits::version_type;
  using allocator_type = Allocator;
  using size_type = dense_container_type::size_type;
  using difference_type = dense_container_type::difference_type;
  using const_reference = dense_container_type::const_reference;
  using const_pointer = dense_container_type::const_pointer;
  using const_iterator = detail::entity_set_iterator<dense_container_type>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  basic_entity_set();

  explicit basic_entity_set(const allocator_type& allocator);

  basic_entity_set(const basic_entity_set& other) = delete;

  basic_entity_set(basic_entity_set&& other) noexcept;

  virtual ~basic_entity_set();

  basic_entity_set& operator=(const basic_entity_set& other) = delete;

  basic_entity_set& operator=(basic_entity_set&& other) noexcept;

  [[nodiscard]] size_type size() const noexcept;

  [[nodiscard]] bool empty() const noexcept;

  [[nodiscard]] const_pointer data() const noexcept;

  [[nodiscard]] const_iterator begin() const noexcept;

  [[nodiscard]] const_iterator cbegin() const noexcept;

  [[nodiscard]] const_iterator end() const noexcept;

  [[nodiscard]] const_iterator cend() const noexcept;

  [[nodiscard]] const_iterator find(const entity_type entity) const noexcept;

  [[nodiscard]] bool contains(const entity_type entity) const noexcept;

  [[nodiscard]] size_type index(const entity_type entity) const noexcept;

  [[nodiscard]] const_reference at(const size_type index) const;

  [[nodiscard]] const_reference operator[](const size_type index) const noexcept;

  const_iterator insert(const entity_type entity);

  void remove(const entity_type entity);

protected:

  virtual void _swap_and_pop(const_iterator first, const_iterator last);

  virtual const_iterator _try_insert(const entity_type entity);

private:

  [[nodiscard]] pointer _sparse_pointer(const entity_type entity) const;

  [[nodiscard]] reference _sparse_reference(const entity_type entity) const;

  [[nodiscard]] reference _assure_at_least(const entity_type entity);

  void _release_sparse_pages();

  sparse_container_type _sparse{};
  dense_container_type _dense{};
  entity_type _free_list{};

}; // class basic_entity_set

template<std::unsigned_integral Type>
using entity_set = basic_entity_set<Type, std::allocator<Type>>;

} // namespace s

#include "entity_set.inl"

#endif // SBX_ECS_ENTITY_SET_HPP_
