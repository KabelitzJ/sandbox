#ifndef SBX_ECS_COMPONENT_CONTAINER_HPP_
#define SBX_ECS_COMPONENT_CONTAINER_HPP_

#include <memory>
#include <vector>

#include <container/sparse_set.hpp>

#include <meta/concepts.hpp>

#include "entity_traits.hpp"

namespace sbx {

template<entity Entity, typename Component, allocator<Component> Allocator, std::size_t PageSize, Entity Placeholder>
class basic_component_container : public basic_sparse_set<typename entity_traits<Entity>::entity_type, typename std::allocator_traits<Allocator>::rebind_alloc<typename entity_traits<Entity>::entity_type>, PageSize, entity_traits<Entity>::to_integral(Placeholder)> {

  using allocator_traits = std::allocator_traits<Allocator>;
  using entity_traits = entity_traits<Entity>;

  using underlying_type = basic_sparse_set<typename entity_traits::entity_type, typename allocator_traits::rebind_alloc<typename entity_traits::entity_type>, PageSize,  entity_traits::to_integral(Placeholder)>;
  using container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<typename allocator_traits::pointer>>;

public:

  using base_type = underlying_type;
  using allocator_type = typename allocator_traits::allocator_type;
  using value_type = typename container_type::value_type;
  using entity_type = typename entity_traits::entity_type;
  using version_type = typename entity_traits::version_type;
  using pointer = typename container_type::pointer;
  using const_pointer = typename container_type::const_pointer;
  using size_type = typename base_type::size_type;

  basic_component_container();

  explicit basic_component_container(const allocator_type& allocator);

  basic_component_container(const basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>& other) = delete;

  basic_component_container(const basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>&& other) noexcept;

  ~basic_component_container() override;

  basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>& operator=(const basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>& other) = delete;

  basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>& operator=(const basic_component_container<Entity, Component, Allocator, PageSize, Placeholder>&& other) noexcept;

  // [[nodiscard]] allocator_type get_allocator() const noexcept;

  // [[nodiscard]] size_type size() const noexcept;

  // [[nodiscard]] bool empty() const noexcept;

  // [[nodiscard]] size_type capacity() const noexcept override;

  // void reserve(const size_type capacity) override;

  // void shrink_to_fit() override;

private:

  container_type _packed{};
  allocator_type _allocator{};

}; // class basic_component_container

template<typename Component>
using component_container = basic_component_container<default_entity, Component, std::allocator<Component>, std::size_t{4096}, placeholder_entity>;

} // namespace sbx

#include "component_container.inl"

#endif // SBX_ECS_COMPONENT_CONTAINER_HPP_
