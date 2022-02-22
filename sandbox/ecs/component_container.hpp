#ifndef SBX_ECS_COMPONENT_CONTAINER_HPP_
#define SBX_ECS_COMPONENT_CONTAINER_HPP_

#include <type_traits>

#include <container/sparse_set.hpp>

#include <meta/concepts.hpp>

namespace sbx {

template<typename Component>
concept component = std::is_default_constructible_v<Component> && std::is_copy_assignable_v<Component>;

template<typename Entity, component Component, allocator<Component> Allocator>
class basic_component_container : basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::rebind_alloc<Entity>> {

  using allocator_traits = std::allocator_traits<Allocator>;

  using base_type = basic_sparse_set<Entity, typename allocator_traits::rebind_alloc<Entity>>;
  using container_type = std::vector<Component, Allocator>;

public:

  using entity_type = Entity;
  using value_type = Component;
  using allocator_type = Allocator;
  using size_type = container_type::size_type;
  using reference = container_type::reference;
  using const_reference = container_type::const_reference;  
  using const_iterator = container_type::const_iterator;  
  using const_reverse_iterator = container_type::const_reverse_iterator;

  basic_component_container() = default;

  explicit basic_component_container(const allocator_type& allocator);

  basic_component_container(const basic_component_container& other) = delete;

  basic_component_container(basic_component_container&& other);

  ~basic_component_container() = default;

  basic_component_container& operator=(const basic_component_container& other) = delete;

  basic_component_container& operator=(basic_component_container&& other);

  /**
   * @brief Returns an iterator to the first element of the container.
   * 
   * @return An iterator to the first element of the container.
   */
  [[nodiscard]] const_iterator begin() const noexcept;

  /**
   * @brief Returns an iterator to the first element of the container.
   * 
   * @return An iterator to the first element of the container.
   */
  [[nodiscard]] const_iterator cbegin() const noexcept;

  /**
   * @brief Returns an iterator to the element following the last element of the container.
   * 
   * @return An iterator to the element following the last element of the container.
   */
  [[nodiscard]] const_iterator end() const noexcept;

  /**
   * @brief Returns an iterator to the element following the last element of the container.
   * 
   * @return An iterator to the element following the last element of the container.
   */
  [[nodiscard]] const_iterator cend() const noexcept;

  /**
   * @brief Returns the number of elements in the container.
   * 
   * @return The number of elements in the container.
   */
  [[nodiscard]] size_type size() const noexcept;

  /**
   * @brief Checks whether the container holds a component for the given entity.
   *
   * @param entity The entity to check. 
   * 
   * @return True if the container holds a component for the given entity, false otherwise.  
   */
  [[nodiscard]] bool contains(const entity_type& entity) const noexcept;

  /**
   * @brief Returns the component for the given entity.
   *
   * @param entity The entity to get the component for.
   * 
   * @return reference The component for the given entity.
   */
  [[nodiscard]] reference operator[](const entity_type& entity);

  /**
   * @brief Returns the component for the given entity.
   *
   * @param entity The entity to get the component for.
   * 
   * @return const_reference The component for the given entity.
   */
  [[nodiscard]] const_reference operator[](const entity_type& entity) const;

  /**
   * @brief Returns the component for the given entity.
   *
   * @param entity The entity to get the component for.
   * 
   * @return reference The component for the given entity.
   */
  [[nodiscard]] reference at(const entity_type& entity);

  /**
   * @brief Returns the component for the given entity.
   *
   * @param entity The entity to get the component for.
   * 
   * @return const_reference The component for the given entity.
   */
  [[nodiscard]] const_reference at(const entity_type& entity) const;

  /**
   * @brief Checks whether the container is empty.
   * 
   * @return True if the container is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const noexcept;

  /**
   * @brief Emplaces a new element at the end of the container.
   *
   * @param entity The entity to associate the component to. 
   * @param args The arguments to use to construct the component.
   *
   * @return A reference to the emplaced component.
   */
  template<typename... Args>
  value_type& emplace(const entity_type entity, Args&&... args);

  /**
   * @brief Removes the component associated with the given entity.
   *
   * @param entity The entity to remove the component from.
   */
  void remove(const entity_type entity);

  /** @brief Removes all elements from the container. */
  void clear();

private:

  container_type _dense{};

}; // class basic_component_container

template<typename Entity, typename Component>
using component_container = basic_component_container<Entity, Component, std::allocator<Component>>;

} // namespace sbx

#include "component_container.inl"

#endif // SBX_ECS_COMPONENT_CONTAINER_HPP_
