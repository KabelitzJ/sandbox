#ifndef SBX_ECS_STORAGE_HPP_
#define SBX_ECS_STORAGE_HPP_

#include <vector>
#include <tuple>
#include <type_traits>

#include <meta/concepts.hpp>

#include "entity_traits.hpp"
#include "entity_set.hpp"

namespace sbx {

namespace detail {

template<container Container, std::size_t PageSize>
class component_map_iterator {

  using container_type = std::remove_const_t<Container>;
  using allocator_traits = std::allocator_traits<typename container_type::allocator_type>;

  using iterator_traits = std::iterator_traits<std::conditional_t<
    std::is_const_v<Container>,
    typename allocator_traits::rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::const_pointer,
    typename allocator_traits::rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::pointer
  >>;

  inline static constexpr auto page_size = PageSize;

public:

  using size_type = container_type::size_type;
  using difference_type = iterator_traits::difference_type;
  using value_type = iterator_traits::value_type;
  using pointer = iterator_traits::pointer;
  using reference = iterator_traits::reference;
  using iterator_category = std::bidirectional_iterator_tag;

  component_map_iterator(const container_type& container, const difference_type offset) noexcept;

  ~component_map_iterator() = default;

  component_map_iterator& operator++() noexcept;

  component_map_iterator operator++(int) noexcept;

  component_map_iterator& operator--() noexcept;

  component_map_iterator operator--(int) noexcept;

  component_map_iterator& operator+=(const difference_type offset) noexcept;

  component_map_iterator operator+(const difference_type offset) const noexcept;

  component_map_iterator& operator-=(const difference_type offset) noexcept;

  component_map_iterator operator-(const difference_type offset) const noexcept;

  [[nodiscard]] reference operator*() const;

  [[nodiscard]] pointer operator->() const;

  [[nodiscard]] size_type index() const noexcept;

private:

  friend class component_map_iterator<const Container, PageSize>;

  container_type* _container{};
  difference_type _offset{};

}; // struct component_map_iterator

template<container Container, std::size_t PageSize>
constexpr bool operator==(const component_map_iterator<Container, PageSize>& lhs, const component_map_iterator<Container, PageSize>& rhs) noexcept;

template<container Container, std::size_t PageSize>
constexpr std::strong_ordering operator<=>(const component_map_iterator<Container, PageSize>& lhs, const component_map_iterator<Container, PageSize>& rhs) noexcept;

} // namespace detail

template<typename Type>
concept component = !std::is_abstract_v<Type>;

template<entity Entity, component Component, allocator<Component> Allocator>
class basic_component_map : public basic_entity_set<Entity, typename std::allocator_traits<Allocator>::rebind_alloc<Entity>> {

  using allocator_traits = std::allocator_traits<Allocator>;
  using entity_traits = entity_traits<Entity>;

  using base_type = basic_entity_set<Entity, typename allocator_traits::rebind_alloc<Entity>>;

  using container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<typename allocator_traits::pointer>>;

  using pointer = Component*;
  using const_pointer = const Component*;

  inline static constexpr auto page_size = std::size_t{1024};

public:

  using allocator_type = Allocator;
  using value_type = Component;
  using entity_type = Entity;
  using size_type = container_type::size_type;
  using difference_type = container_type::difference_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = detail::component_map_iterator<container_type, page_size>;
  using const_iterator = detail::component_map_iterator<const container_type, page_size>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  basic_component_map();

  explicit basic_component_map(const allocator_type& allocator);

  basic_component_map(const basic_component_map& other) = delete;

  basic_component_map(basic_component_map&& other) noexcept;

  ~basic_component_map() override;

  basic_component_map& operator=(const basic_component_map& other) = delete;

  basic_component_map& operator=(basic_component_map&& other) noexcept;

  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept;

  [[nodiscard]] constexpr size_type size() const noexcept;

  [[nodiscard]] constexpr bool empty() const noexcept;

  template<typename... Args>
  requires (std::is_constructible_v<Component, Args...>)
  value_type& emplace(const entity_type entity, Args&&... args);

  [[nodiscard]] const value_type& get(const entity_type entity) const;

  [[nodiscard]] value_type& get(const entity_type entity);

  [[nodiscard]] std::tuple<const value_type&> get_as_tuple(const entity_type entity) const;

  [[nodiscard]] std::tuple<value_type&> get_as_tuple(const entity_type entity);

protected:

  using base_type_iterator = base_type::const_iterator;

  void _swap_and_pop(base_type_iterator first, base_type_iterator last) override;

  base_type_iterator _try_insert(const entity_type entity) override;

private:

  [[nodiscard]] reference _element_at(const size_type index) const;

  pointer _assure_at_least(const size_type index);

  template<typename... Args>
  requires (std::is_constructible_v<Component, Args...>)
  base_type::const_iterator _emplace_component(const entity_type entity, Args&&... args);

  void _shrink_to_fit(const size_type size);

  allocator_type _page_allocator{};
  container_type _dense{};

}; // class basic_component_map

template<entity Entity, component Component>
using component_map = basic_component_map<Entity, Component, std::allocator<Component>>;

} // namespace sbx

#include "component_map.inl"

#endif // SBX_ECS_STORAGE_HPP_
