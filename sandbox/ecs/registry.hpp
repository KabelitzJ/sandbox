#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>

#include <container/dynamic_bitset.hpp>

#include "entity.hpp"
#include "entity_set.hpp"
#include "component_storage.hpp"
#include "view.hpp"

namespace sbx {

class registry {

public:

  using size_type = std::size_t;

  registry() = default;

  registry(const registry&) = delete;

  registry(registry&&) noexcept;

  ~registry() = default;

  registry& operator=(const registry&) = delete;

  registry& operator=(registry&&) noexcept;

  [[nodiscard]] entity create_entity();

  void destroy_entity(const entity& entity);

  [[nodiscard]] bool is_valid_entity(const entity& entity) const noexcept;

  template<typename Component, typename... Args>
  requires (std::is_constructible_v<Component, Args...> && !std::is_const_v<Component> && std::is_same_v<std::decay_t<Component>, Component>)
  Component& add_component(const entity& entity, Args&&... args);

  template<typename Component>
  void remove_component(const entity& entity);

  template<typename Component>
  [[nodiscard]] bool has_component(const entity& entity) const;

  template<typename Component>
  [[nodiscard]] const Component& get_component(const entity& entity) const;

  template<typename Component>
  [[nodiscard]] Component& get_component(const entity& entity);

  template<typename... Components, typename Function>
  requires (std::is_invocable_r_v<void, Function, const entity&, Components&...>)
  void for_all(Function&& function);

  template<typename... Components>
  [[nodiscard]] view<Components...> create_view();

private:

  template<typename Component>
  size_type _component_id() const noexcept {
    static auto id = _current_component_id++;
    return id;
  }

  std::vector<entity> _entities{};
  std::queue<size_type> _free_entities{};
  std::vector<std::unique_ptr<entity_set>> _component_containers{};
  mutable size_type _current_component_id{};

}; // class registry

} // namespace sbx

#include "registry.inl"

#endif // SBX_ECS_REGISTRY_HPP_
