#include <ranges>
#include <functional>
#include <algorithm>

namespace sbx {

template<typename Component, typename... Args>
requires (std::is_constructible_v<Component, Args...> && !std::is_const_v<Component> && std::is_same_v<std::decay_t<Component>, Component>)
Component& registry::add_component(const entity& entity, Args&&... args) {
  if (has_component<Component>(entity)) {
    throw std::runtime_error("entity already has component");
  }

  using no_cv_component_type = std::remove_cv_t<Component>;

  const auto component_id = _component_id<no_cv_component_type>();

  if (component_id >= _component_containers.size()) {
    _component_containers.emplace_back(std::make_unique<component_container<no_cv_component_type>>());
  }

  auto& component = *static_cast<component_container<no_cv_component_type>*>(_component_containers[component_id].get());

  return component.emplace(entity, std::forward<Args>(args)...);
}

template<typename Component>
void registry::remove_component(const entity& entity) {
  if (!has_component<Component>(entity)) {
    return;
  }

  using no_cv_component_type = std::remove_cv_t<Component>;

  const auto component_id = _component_id<no_cv_component_type>();

  auto& component = *static_cast<component_container<no_cv_component_type>*>(_component_containers[component_id].get());

  component.erase(entity);
}

template<typename Component>
bool registry::has_component(const entity& entity) const {
  if (!is_valid_entity(entity)) {
    throw std::invalid_argument("entity is invalid");
  }

  using no_cv_component_type = std::remove_cv_t<Component>;

  const auto component_id = _component_id<no_cv_component_type>();

  if (component_id >= _component_containers.size()) {
    return false;
  }

  auto& component = *static_cast<component_container<no_cv_component_type>*>(_component_containers[component_id].get());

  return component.contains(entity);
}

template<typename Component>
const Component& registry::get_component(const entity& entity) const {
  if (!has_component<Component>(entity)) {
    throw std::invalid_argument("entity does not have component");
  }

  using no_cv_component_type = std::remove_cv_t<Component>;

  const auto component_id = _component_id<no_cv_component_type>();

  const auto& component = *static_cast<component_container<no_cv_component_type>*>(_component_containers[component_id].get());

  return component.get(entity);
}

template<typename Component>
Component& registry::get_component(const entity& entity) {
  return const_cast<Component&>(std::as_const(*this).get_component<Component>(entity));
}

template<typename... Components, typename Function>
requires (std::is_invocable_r_v<void, Function, const entity&, Components&...>)
void registry::for_all(Function&& function) {
  if constexpr (sizeof...(Components) == size_type{0}) {
    for (auto& entity : _entities) {
      std::invoke(function, entity);
    }
  } else {
    const auto component_filter = [&](const auto& entity){
      const auto has_components = std::initializer_list<bool>{has_component<Components>(entity)...};
      return std::all_of(has_components.begin(), has_components.end(), std::identity{});
    };

    for (const auto& entity : _entities | std::views::filter(component_filter)) {
      std::apply(std::forward<Function>(function), std::forward_as_tuple(entity, get_component<Components>(entity)...));
    }
  }

}

template<typename... Components>
view<Components...> registry::create_view() {
  if constexpr (sizeof...(Components) == size_type{0}) {
    return view<Components...>{};
  } else {
    using view_container_type = view<Components...>::container_type;

    const auto component_filter = [&](const auto& entity){
      const auto has_components = std::initializer_list<bool>{has_component<Components>(entity)...};
      return std::all_of(has_components.begin(), has_components.end(), std::identity{});
    };

    auto view_entries = view_container_type{};

    for (const auto& entity : _entities | std::views::filter(component_filter)) {
      view_entries.emplace_back(std::forward_as_tuple(entity, get_component<Components>(entity)...));
    }

    return view<Components...>{view_entries};
  }
}

} // namespace sbx
