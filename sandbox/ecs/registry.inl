#include "registry.hpp"

#include <ranges>

#include <memory/pool_allocator.hpp>

namespace sbx {

// [TODO] KAJ 2022-07-17 20:38 - Find a way to type alias 'component_container<Component, pool_allocator<Component, 64>>'

template<component Component, typename... Args>
inline Component& registry::add_component(const entity& entity, Args&&... args) {
  if (!is_valid_entity(entity)) {
    throw std::runtime_error{"Invalid entity"};
  }

  const auto component_id = _component_id<Component>();

  if (component_id == _component_containers.size()) {
    const auto deleter = [](auto* container){
      delete static_cast<component_container<Component, pool_allocator<Component, 64>>*>(container);
    };

    auto container = std::unique_ptr<void, void(*)(void*)>{new component_container<Component, pool_allocator<Component, 64>>{}, deleter};

    _component_containers.push_back(std::move(container));
  }

  auto& container = *static_cast<component_container<Component, pool_allocator<Component, 64>>*>(_component_containers[component_id].get());

  return container.add(entity, std::forward<Args>(args)...);
}

template<component Component>
inline void registry::remove_component(const entity& entity) {
  if (!is_valid_entity(entity)) {
    throw std::runtime_error{"Invalid entity"};
  }

  const auto component_id = _component_id<Component>();

  auto& container = *static_cast<component_container<Component, pool_allocator<Component, 64>>*>(_component_containers[component_id].get());

  container.remove(entity);
}

template<component Component>
inline bool registry::has_component(const entity& entity) const {
  if (!is_valid_entity(entity)) {
    throw std::runtime_error{"Invalid entity"};
  }

  const auto component_id = _component_id<Component>();

  const auto& container = *static_cast<component_container<Component, pool_allocator<Component, 64>>*>(_component_containers[component_id].get());

  return container.contains(entity);
}

template<component Component>
inline Component& registry::get_component(const entity& entity) {
  if (!is_valid_entity(entity)) {
    throw std::runtime_error{"Invalid entity"};
  }

  const auto component_id = _component_id<Component>();

  if constexpr (std::is_const_v<Component>) {
    const auto& container = *static_cast<component_container<Component, pool_allocator<Component, 64>>*>(_component_containers[component_id].get());
    return container.get(entity);
  } else {
    auto& container = *static_cast<component_container<Component, pool_allocator<Component, 64>>*>(_component_containers[component_id].get());
    return container.get(entity);
  }
}

template<component Component, typename Function>
inline void registry::patch_component(const entity& entity, Function&& function) {
  if (!is_valid_entity(entity)) {
    throw std::runtime_error{"Invalid entity"};
  }

  const auto component_id = _component_id<Component>();

  auto& container = *static_cast<component_container<Component, pool_allocator<Component, 64>>*>(_component_containers[component_id].get());
  return container.patch(entity, std::forward<Function>(function));
}

template<component... Components>
inline view<Components...> registry::create_view() {
  if constexpr (sizeof...(Components) == size_type{0}) {
    return view<Components...>{};
  } else {
    using view_container_type = view<Components...>::container_type;

    const auto component_filter = [&](const auto& entity){
      const auto has_components = std::initializer_list{has_component<Components>(entity)...};
      return std::all_of(has_components.begin(), has_components.end(), std::identity{});
    };

    auto view_entries = view_container_type{};

    for (const auto& entity : _entities | std::views::filter(component_filter)) {
      view_entries.emplace_back(std::forward_as_tuple(entity, get_component<Components>(entity)...));
    }

    return view<Components...>{view_entries};
  }
}

template<component Component>
inline registry::size_type registry::_component_id() const {
  // [NOTE] KAJ 2022-07-01 16:27 - Const and non-const types should be stored in the same container
  return _generate_next_component_id<std::remove_const_t<Component>>(); 
}

template<component Component>
inline registry::size_type registry::_generate_next_component_id() const {
  static const auto id = _current_component_id++;
  return id;
}

} // namespace sbx
