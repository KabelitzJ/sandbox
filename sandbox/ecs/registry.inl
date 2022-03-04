#include <ranges>
#include <algorithm>

namespace sbx {

template<component Component, typename... Args>
requires (std::is_constructible_v<Component, Args...>)
inline Component& registry::add_component(const entity& entity, Args&&... args) {
  if (has_component<Component>(entity)) {
    return get_component<Component>(entity);
  }

  const auto component_id = std::type_index{typeid(Component)};

  if (!_components.contains(component_id)) {
    _components.emplace(component_id, component_container{});
  }

  auto& container = _components.at(component_id);

  auto deleter = [](auto* pointer){ delete static_cast<Component*>(pointer); };

  auto component = component_handle{new Component{std::forward<Args>(args)...}, deleter};

  container.emplace(entity._id(), std::move(component));

  return *static_cast<Component*>(container.at(entity._id()).get());
}

template<component Component>
inline bool registry::has_component(const entity& entity) const noexcept {
  const auto component_id = std::type_index{typeid(Component)};
  
  if (const auto& container = _components.find(component_id); container != _components.cend()) {
    return container->second.contains(entity._id());
  }

  return false;
}

template<component Component>
inline const Component& registry::get_component(const entity& entity) const {
  if (!has_component<Component>(entity)) {
    throw std::runtime_error{"entity does not have component"};
  }

  const auto component_id = std::type_index{typeid(Component)};

  const auto& container = _components.at(component_id);

  return *static_cast<Component*>(container.at(entity._id()).get());
}

template<component Component>
inline Component& registry::get_component(const entity& entity) {
  return const_cast<Component&>(std::as_const(*this).get_component<Component>(entity));
}

template<component Component>
inline void registry::remove_component(const entity& entity) {
  if (!has_component<Component>(entity)) {
    return;
  }

  const auto component_id = std::type_index{typeid(Component)};

  auto& container = _components.at(component_id);

  container.erase(entity._id());
}

template<component... Components>
inline void registry::create_view() {

}

} // namespace sbx
