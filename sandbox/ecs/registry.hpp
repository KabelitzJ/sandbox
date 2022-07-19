#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <vector>
#include <queue>
#include <memory>
#include <iostream>
#include <typeindex>

#include <utils/noncopyable.hpp>

#include <memory/pool_allocator.hpp>

#include "entity.hpp"
#include "entity_set.hpp"
#include "component_container.hpp"
#include "view.hpp"

namespace sbx {

class registry : public noncopyable {

  using container_base_type = entity_set;

  template<component Component>
  using component_container_type = component_container<Component, sbx::pool_allocator<Component, 256>>;

public:

  using size_type = std::size_t;

  registry() = default;

  ~registry() = default;

  [[nodiscard]] entity create_entity();

  void destroy_entity(const entity& entity);

  [[nodiscard]] bool is_valid_entity(const entity& entity) const noexcept;

  template<component Component, typename... Args>
  Component& add_component(const entity& entity, Args&&... args);

  template<component Component>
  void remove_component(const entity& entity);

  template<component Component>
  [[nodiscard]] bool has_component(const entity& entity) const;

  template<component Component>
  [[nodiscard]] Component& get_component(const entity& entity);

  template<component Component, typename Function>
  void patch_component(const entity& entity, Function&& function);

  template<component... Components>
  [[nodiscard]] view<Components...> create_view();

private:

  template<component Component>
  [[nodiscard]] size_type _component_id() const;

  template<component Component>
  [[nodiscard]] size_type _generate_next_component_id() const;

  std::vector<entity> _entities{};
  std::vector<size_type> _free_entities{};
  std::vector<std::unique_ptr<container_base_type>> _component_containers{};
  mutable size_type _current_component_id{};

}; // class registry;

} // namespace sbx

#include "registry.inl"

#endif // SBX_ECS_REGISTRY_HPP_
