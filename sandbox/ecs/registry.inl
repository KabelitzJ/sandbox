#include <functional>

namespace sbx {

template<typename Entity, allocator<Entity> Allocator>
template<component Component, typename... Args>
requires (std::is_constructible_v<Component, Args...>)
Component& basic_registry<Entity, Allocator>::add_component(const entity_type entity, Args&&... args) {
  constexpr auto id = type_id<Component>{};

  auto& container = _component_containers[id];

  if (!container) {
    container.reset(new component_container_type<Component>{});
  }

  auto& concrete_container = static_cast<component_container_type<Component>&>(*container);

  return concrete_container.emplace(entity, std::forward<Args>(args)...);
}

template<typename Entity, allocator<Entity> Allocator>
template<component Component>
void basic_registry<Entity, Allocator>::remove_component(const entity_type entity) {
  constexpr auto id = type_id<Component>{};

  auto& container = _component_containers[id];

  if (!container) {
    return;
  }

  auto& concrete_container = static_cast<component_container_type<Component>&>(*container);

  concrete_container.remove(entity);
}

} // namespace sbx
