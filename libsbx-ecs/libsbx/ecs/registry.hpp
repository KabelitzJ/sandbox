#ifndef LIBSBX_ECS_REGISTRY_HPP_
#define LIBSBX_ECS_REGISTRY_HPP_

#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <optional>
#include <algorithm>
#include <tuple>
#include <ranges>

#include <libsbx/memory/concepts.hpp>

#include <libsbx/ecs/entity.hpp>
#include <libsbx/ecs/sparse_set.hpp>
#include <libsbx/ecs/storage.hpp>
#include <libsbx/ecs/view.hpp>

namespace sbx::ecs {

template<typename Entity, memory::allocator_for<Entity> Allocator = std::allocator<Entity>>
requires (utility::is_complete_v<entity_traits<Entity>>)
class basic_registry {

  using allocator_traits = std::allocator_traits<Allocator>;

  using entity_storage_type = std::vector<Entity, Allocator>;
  using free_list_type = std::vector<std::size_t, memory::rebound_allocator_t<Allocator, std::size_t>>;

  using basic_storage_type = sparse_set<Entity, Allocator>;

  template<typename Type>
  using storage_type = storage<Entity, Type, memory::rebound_allocator_t<Allocator, Type>>;

public:

  using entity_type = Entity;
  using allocator_type = Allocator;
  using size_type = std::size_t;

  template<typename... Components>
  using view_type = basic_view<Components...>;

  basic_registry() = default;

  basic_registry(const basic_registry&) = delete;

  basic_registry(basic_registry&& other) noexcept
  : _entities{std::move(other._entities)},
    _free_entities{std::move(other._free_entities)},
    _component_storages{std::move(other._component_storages)} { }

  ~basic_registry() {
    clear();
  }

  auto operator=(const basic_registry&) -> basic_registry& = delete;

  auto operator=(basic_registry&& other) noexcept -> basic_registry& {
    if (this != &other) {
      _entities = std::move(other._entities);
      _free_entities = std::move(other._free_entities);
      _component_storages = std::move(other._component_storages);
    }

    return *this;
  }

  auto clear() -> void {
    for (auto& [key, storage] : _component_storages) {
      storage->clear();
    }

    _entities.clear();
    _free_entities.clear();
  }

  auto create_entity() -> entity_type {
    if (!_free_entities.empty()) {
      auto index = _free_entities.back();
      _free_entities.pop_back();

      return _entities.at(index);
    }

    const auto id = static_cast<entity_type::id_type>(_entities.size());

    auto new_entity = entity{id, static_cast<entity_type::version_type>(0)};

    _entities.push_back(new_entity);
    return new_entity;
  }

  auto destroy_entity(const entity_type& entity) -> void {
    auto index = static_cast<std::size_t>(entity._id());
    _free_entities.push_back(index);
    _entities.at(index)._increment_version();
  }

  auto is_valid_entity(const entity_type& entity) -> bool {
    auto index = static_cast<std::size_t>(entity._id());
    return index < _entities.size() && entity == _entities.at(index);
  }

  template<typename Component>
  auto has_component(const entity_type& entity) const -> bool {
    using component_type = std::remove_cvref_t<Component>;

    if (const auto storage = _try_get_storage<component_type>(); storage) {
      return storage->get().contains(entity);
    }

    return false;
  }

  template<typename Component, typename... Args>
  auto add_component(const entity_type& entity, Args&&... args) -> Component& {
    using component_type = std::remove_cvref_t<Component>;

    auto& storage = _get_or_create_storage<component_type>();

    return static_cast<storage_type<component_type>&>(storage).add(entity, std::forward<Args>(args)...);
  }

  template<typename Component>
  auto get_component(const entity_type& entity) const -> const Component& {
    using component_type = std::remove_cvref_t<Component>;

    if (const auto component = try_get_component<component_type>(entity); component) {
      return component->get();
    }

    throw std::runtime_error{"Entity does not have component assigned to it"};
  }

  template<typename Component>
  auto get_component(const entity_type& entity) -> Component& {
    using component_type = std::remove_cvref_t<Component>;

    if (auto component = try_get_component<component_type>(entity); component) {
      return component->get();
    }

    throw std::runtime_error{"Entity does not have component assigned to it"};
  }

  template<typename Component>
  auto try_get_component(const entity_type& entity) const -> std::optional<std::reference_wrapper<const Component>> {
    using component_type = std::remove_cvref_t<Component>;

    if (const auto storage = _try_get_storage<component_type>(); storage) {
      const auto& component_storage = static_cast<const storage_type<component_type>&>(storage->get());

      if (auto entry = component_storage.find(entity); entry != component_storage.cend()) {
        return std::cref(*entry);
      }
    }

    throw std::nullopt;
  }

  template<typename Component>
  auto try_get_component(const entity_type& entity) -> std::optional<std::reference_wrapper<Component>> {
    using component_type = std::remove_cvref_t<Component>;

    if (auto storage = _try_get_storage<component_type>(); storage) {
      auto& component_storage = static_cast<storage_type<component_type>&>(storage->get());

      if (auto entry = component_storage.find(entity); entry != component_storage.end()) {
        return std::ref(*entry);
      }
    }

    throw std::nullopt;
  }

  template<typename... Components>
  auto create_view() -> view_type<Components...> {
    if constexpr (sizeof...(Components) == 0) {
      return view_type<Components...>{};
    } else {
      using container_type = typename view_type<Components...>::container_type;

      const auto component_filter = [&](const auto& entity){
        const auto has_components = std::initializer_list{has_component<Components>(entity)...};
      
        return std::all_of(std::begin(has_components), std::end(has_components), std::identity{});
      };

      auto entries = container_type{};

      for (const auto& entity : _entities | std::views::filter(component_filter)) {
        entries.emplace_back(std::forward_as_tuple(entity, get_component<Components>(entity)...));
      }

      return view_type<Components...>{entries};
    }
  }

private:

  template<typename Component>
  auto _get_or_create_storage() -> basic_storage_type& {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _component_storages.find(type); entry != _component_storages.end()) {
      return *entry->second;
    }

    auto entry = _component_storages.insert({type, std::make_unique<storage_type<Component>>()}).first;

    return *entry->second;
  }

  template<typename Component>
  auto _try_get_storage() const -> std::optional<std::reference_wrapper<const basic_storage_type>> {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _component_storages.find(type); entry != _component_storages.cend()) {
      return std::cref(*entry->second);
    }

    return std::nullopt;
  }

  template<typename Component>
  auto _try_get_storage() -> std::optional<std::reference_wrapper<basic_storage_type>> {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _component_storages.find(type); entry != _component_storages.end()) {
      return std::ref(*entry->second);
    }

    return std::nullopt;
  }

  entity_storage_type _entities;
  free_list_type _free_entities;

  std::unordered_map<std::type_index, std::unique_ptr<basic_storage_type>> _component_storages;

};

using registry = basic_registry<entity>;

} // namespace sbx::ecs

#endif // LIBSBX_ECS_REGISTRY_HPP_
