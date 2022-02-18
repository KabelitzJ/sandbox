#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <iterator>
#include <memory>
#include <vector>

#include <meta/concepts.hpp>

#include "entity_traits.hpp"
#include "entity.hpp"

namespace sbx {

namespace detail {

template<container Container>
struct sparse_set_iterator {

  using value_type = typename Container::value_type;
  using pointer = typename Container::pointer;
  using reference = typename Container::reference;
  using difference_type = typename Container::difference_type;
  using size_type = typename Container::size_type;
  using iterator_category = std::random_access_iterator_tag;

  sparse_set_iterator() noexcept = default;

  sparse_set_iterator(const Container& container, const difference_type offset) noexcept;

  sparse_set_iterator(const sparse_set_iterator<Container>& other) noexcept = default;

  sparse_set_iterator(sparse_set_iterator<Container>&& other) noexcept = default;

  ~sparse_set_iterator() noexcept = default;

  sparse_set_iterator<Container>& operator=(const sparse_set_iterator<Container>& other) noexcept = default;

  sparse_set_iterator<Container>& operator=(sparse_set_iterator<Container>&& other) noexcept = default;

  sparse_set_iterator<Container>& operator++() noexcept;

  sparse_set_iterator<Container> operator++(int) noexcept;

  sparse_set_iterator<Container>& operator--() noexcept;

  sparse_set_iterator<Container> operator--(int) noexcept;

  sparse_set_iterator<Container>& operator+=(const difference_type offset) noexcept;

  sparse_set_iterator<Container> operator+(const difference_type offset) const noexcept;

  sparse_set_iterator<Container>& operator-=(const difference_type offset) noexcept;

  sparse_set_iterator<Container> operator-(const difference_type offset) const noexcept;

  [[nodiscard]] reference operator[](const size_type index) const;

  [[nodiscard]] reference operator*() const;

  [[nodiscard]] pointer operator->() const;

  [[nodiscard]] difference_type index() const noexcept;

private:

  const Container* _container{};
  difference_type _offset{};

};

template<container Container>
[[nodiscard]] typename sparse_set_iterator<Container>::difference_type operator-(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept;

template<container Container>
[[nodiscard]] bool operator==(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept;

template<container Container>
[[nodiscard]] std::strong_ordering operator<=>(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept;

} // namespace detail

template<entity Entity, allocator<Entity> Allocator>
class basic_sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;
  using entity_traits = entity_traits<Entity>;
  using sparse_container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::template rebind_alloc<typename allocator_traits::pointer>>;
  using packed_container_type = std::vector<typename allocator_traits::value_type, typename allocator_traits::allocator_type>;

  inline static constexpr auto page_size = std::size_t{4096};

public:

  using allocator_type = typename allocator_traits::allocator_type;
  using value_type = typename allocator_traits::value_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using entity_type = typename entity_traits::entity_type;
  using version_type = typename entity_traits::version_type;
  using size_type = typename packed_container_type::size_type;
  using pointer = typename packed_container_type::const_pointer;

protected:

  using basic_iterator = detail::sparse_set_iterator<packed_container_type>;

private:

  [[nodiscard]] pointer _sparse_page(const entity_type entity) const;

  [[nodiscard]] reference _sparse_entry(const entity_type entity) const;

  [[nodiscard]] reference _assure_page(const entity_type entity);

  void _release_page_pages();

  sparse_container_type _sparse{};
  packed_container_type _packed{};
  entity_type _free_list{};

}; // class basic_sparse_set

using sparse_set = basic_sparse_set<default_entity, std::allocator<default_entity>>;

} // namespace sbx

#include "sparse_set.inl"

#endif // SBX_ECS_SPARSE_SET_HPP_
