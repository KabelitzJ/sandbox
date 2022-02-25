#ifndef SBX_ECS_STORAGE_HPP_
#define SBX_ECS_STORAGE_HPP_

#include <vector>
#include <type_traits>

#include <container/sparse_set.hpp>

#include <meta/concepts.hpp>

#include "entity_traits.hpp"

namespace sbx {

template<typename Type>
concept component = !std::is_abstract_v<Type>;

template<entity Entity, component Component, allocator<Component> Allocator>
class basic_storage : public basic_sparse_set<typename entity_traits<Entity>::entity_type, typename std::allocator_traits<Allocator>::rebind_alloc<typename entity_traits<Entity>::entity_type>> {

  using allocator_traits = std::allocator_traits<Allocator>;
  using entity_traits = entity_traits<Entity>;

  using base_type = basic_sparse_set<typename entity_traits::entity_type, typename allocator_traits::rebind_alloc<typename entity_traits::entity_type>>;

  using dense_container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<typename allocator_traits::pointer>>;

  using pointer = Component*;

  inline static constexpr auto page_size = std::size_t{512};

public:

  using allocator_type = Allocator;
  using value_type = entity_traits::value_type;
  using entity_type = entity_traits::entity_type;
  using size_type = dense_container_type::size_type;
  using difference_type = dense_container_type::difference_type;
  using reference = Component&;
  using const_reference = const Component&;

  basic_storage();

  explicit basic_storage(const allocator_type& allocator);

  basic_storage(const basic_storage& other) = delete;

  basic_storage(basic_storage&& other) noexcept;

  ~basic_storage() override;

  basic_storage& operator=(const basic_storage& other) = delete;

  basic_storage& operator=(basic_storage&& other) noexcept;

private:

  [[nodiscard]] reference _element_at(const size_type index) const;

  pointer _assure_at_least(const size_type index);

  template<typename... Args>
  requires (std::is_constructible_v<Component, Args...>)
  reference _try_emplace(const entity_type entity, Args&&... args);

  void _shrink_to_fit(const size_type size);

  allocator_type _page_allocator{};
  dense_container_type _dense{};

}; // class basic_storage

template<entity Entity, component Component>
using storage = basic_storage<Entity, Component, std::allocator<Component>>;

} // namespace sbx

#include "storage.inl"

#endif // SBX_ECS_STORAGE_HPP_
