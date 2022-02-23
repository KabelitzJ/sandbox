#include <functional>

namespace sbx {

template<typename Entity, allocator<Entity> Allocator>
template<component Component, typename... Args>
Component& basic_registry<Entity, Allocator>::add_component(const entity_type entity, Args&&... args) {
  constexpr auto id = type_id<Component>{};

  auto& container = _component_containers[id];

  if (!container) {
    container.reset(new component_container_type<Component>{});
  }

  auto& concrete_container = static_cast<component_container_type<Component>&>(*container);

  return concrete_container.emplace(entity, std::forward<Args>(args)...);
}

} // namespace sbx
