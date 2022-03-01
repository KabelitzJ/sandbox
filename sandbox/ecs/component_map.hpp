#ifndef SBX_ECS_STORAGE_HPP_
#define SBX_ECS_STORAGE_HPP_

#include <vector>
#include <tuple>
#include <type_traits>

#include <meta/concepts.hpp>

#include "entity_traits.hpp"
#include "entity_set.hpp"

namespace sbx {

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
