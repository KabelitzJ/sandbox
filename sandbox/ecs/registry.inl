#include <ranges>
#include <functional>
#include <algorithm>

namespace sbx {

template<typename Component, typename... Args>
requires (std::is_constructible_v<Component, Args...> && !std::is_const_v<Component> && std::is_same_v<std::decay_t<Component>, Component>)
Component& registry::add_component(const entity& entity, Args&&... args) {
  if (has_component<Component>(entity)) {
    return get_component<Component>(entity);
  }

  const auto component_id = _component_id<Component>();
  const auto entity_value = entity._value;

  auto& container = _components[component_id];

  auto deleter = [](auto* pointer){ 
    delete static_cast<Component*>(pointer); 
  };

  auto component = component_handle{new Component{std::forward<Args>(args)...}, deleter};

  const auto [itr, result] = container.emplace(entity_value, std::move(component));

  const auto index = static_cast<size_type>(entity._id());

  _entities[index].signature.set(component_id);

  auto& handle = itr->second;

  return *static_cast<Component*>(handle.get());
}

template<typename Component>
void registry::remove_component(const entity& entity) {
  if (!has_component<Component>(entity)) {
    return;
  }

  const auto component_id = _component_id<std::remove_const_t<Component>>();
  const auto entity_value = entity._value;

  const auto index = static_cast<size_type>(entity._id());

  _entities[index].signature.reset(component_id);

  _components[component_id].erase(entity_value);
}

template<typename Component>
bool registry::has_component(const entity& entity) const {
  if (!is_valid_entity(entity)) {
    throw std::invalid_argument("entity is invalid");
  }

  const auto component_id = _component_id<std::remove_const_t<Component>>();

  const auto index = static_cast<size_type>(entity._id());

  return _entities[index].signature.test(component_id);
}

template<typename Component>
const Component& registry::get_component(const entity& entity) const {
  if (!has_component<Component>(entity)) {
    throw std::invalid_argument("entity does not have component");
  }

  const auto component_id = _component_id<std::remove_const_t<Component>>();
  const auto entity_value = entity._value;

  const auto& container = _components.at(component_id);

  const auto& handle = container.at(entity_value);

  return *static_cast<const Component*>(handle.get());
}

template<typename Component>
Component& registry::get_component(const entity& entity) {
  return const_cast<Component&>(std::as_const(*this).get_component<Component>(entity));
}

template<typename... Components, typename Function>
requires (std::is_invocable_r_v<void, Function, const entity&, Components&...>)
void registry::for_all(Function&& function) {
  if constexpr (sizeof...(Components) == size_type{0}) {
    for (auto& [entity, signature] : _entities) {
      std::invoke(function, entity);
    }
  } else {
    const auto component_filter = [&](const auto& data){
      const auto has_components = std::initializer_list<bool>{has_component<Components>(data.entity)...};
      return std::all_of(has_components.begin(), has_components.end(), std::identity{});
    };

    for (const auto& [entity, signature] : _entities | std::views::filter(component_filter)) {
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

    const auto component_filter = [&](const auto& data){
      const auto has_components = std::initializer_list<bool>{has_component<Components>(data.entity)...};
      return std::all_of(has_components.begin(), has_components.end(), std::identity{});
    };

    auto view_entries = view_container_type{};

    for (const auto& [entity, signature] : _entities | std::views::filter(component_filter)) {
      view_entries.emplace_back(std::forward_as_tuple(entity, get_component<Components>(entity)...));
    }

    return view<Components...>{view_entries};
  }
}

} // namespace sbx
