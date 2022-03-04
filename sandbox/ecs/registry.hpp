#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <memory>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "component.hpp"
#include "entity.hpp"

namespace sbx {

class registry {

  using component_handle = std::unique_ptr<void, void(*)(void*)>;
  using component_container = std::unordered_map<entity::id_type, component_handle>;

public:

  using size_type = std::size_t;

  registry() noexcept = default;

  registry(const registry& other) = delete;

  registry(registry&& other) noexcept;

  ~registry() noexcept = default;

  registry& operator=(const registry& other) = delete;

  registry& operator=(registry&& other) noexcept;

  [[nodiscard]] entity create_entity();

  void destroy_entity(const entity& entity);

  [[nodiscard]] bool is_valid_entity(const entity& entity) const noexcept;

  template<component Component, typename... Args>
  requires (std::is_constructible_v<Component, Args...>)
  Component& add_component(const entity& entity, Args&&... args);

  template<component Component>
  [[nodiscard]] bool has_component(const entity& entity) const noexcept;

  template<component Component>
  const Component& get_component(const entity& entity) const;

  template<component Component>
  Component& get_component(const entity& entity);

  template<component Component>
  void remove_component(const entity& entity);

  template<component... Components>
  void create_view();

private:

  std::vector<entity> _entities{};
  std::queue<size_type> _free_list{};
  std::unordered_map<std::type_index, component_container> _components{};

};

} // namespace sbx

#include "registry.inl"

#endif // SBX_ECS_REGISTRY_HPP_
