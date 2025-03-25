#ifndef LIBSBX_REGISTRY_HPP_
#define LIBSBX_REGISTRY_HPP_

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <optional>
#include <algorithm>
#include <tuple>
#include <ranges>

#include <libsbx/memory/concepts.hpp>
#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/ecs/entity.hpp>
#include <libsbx/ecs/sparse_set.hpp>
#include <libsbx/ecs/storage.hpp>
#include <libsbx/ecs/view.hpp>

namespace sbx::ecs {

template<typename To, typename From>
struct constness_as {
  using type = std::remove_const_t<To>;
};

template<typename To, typename From>
struct constness_as<To, const From> {
  using type = std::add_const_t<To>;
};

template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;

template<typename... Types>
struct variadic_template_size {
  inline static constexpr auto value = sizeof...(Types);
}; // struct variadic_template_size

template<typename... Types>
constexpr auto variadic_template_size_v = variadic_template_size<Types...>::value;

template<typename Entity, typename EntityList, typename FreeList> 
class registry_iterator {

  using iterator_type = EntityList::const_iterator;

  using entity_traits = ecs::entity_traits<Entity>;


public:

  using value_type = typename iterator_type::value_type;
  using pointer = typename iterator_type::pointer;
  using reference = typename iterator_type::reference;
  using difference_type = typename iterator_type::difference_type;
  using iterator_category = std::forward_iterator_tag;

  registry_iterator(iterator_type current, iterator_type end, FreeList& free_entities)
  : _current{current},
    _end{end},
    _free_entities{std::addressof(free_entities)} {
    while(_current != _end && !_is_valid()) {
      ++_current;
    }
  }

  auto operator++() noexcept -> registry_iterator& {
    while(++_current != _end && !_is_valid()) {}
    return *this;
  }

  auto operator++(int) noexcept -> registry_iterator {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  auto operator->() const noexcept -> pointer {
    return &*_current;
  }

  auto operator*() const noexcept -> reference {
    return *(operator->());
  }

  template<typename LhsEntity, typename LhsEntityList, typename LhsFreeList, typename RhsEntity,  typename RhsEntityList, typename RhsFreeList>
  friend auto operator==(const registry_iterator<LhsEntity, LhsEntityList, LhsFreeList>& lhs, const registry_iterator<RhsEntity, RhsEntityList, RhsFreeList>& rhs) noexcept -> bool {
    return lhs._current == rhs._current;
  } 

private:

  auto _is_valid() const noexcept -> bool {
    return !_free_entities->contains(entity_traits::to_entity(*_current));
  }

  iterator_type _current;
  iterator_type _end;
  const FreeList* _free_entities;

}; // class registry_iterator

template<typename Entity, memory::allocator_for<Entity> Allocator = std::allocator<Entity>>
class basic_registry {

  using allocator_traits = std::allocator_traits<Allocator>;

  using entity_storage_type = std::vector<Entity, Allocator>;
  using free_list_type = std::unordered_set<std::size_t, std::hash<std::size_t>, std::equal_to<std::size_t>, memory::rebound_allocator_t<Allocator, std::size_t>>;

  using basic_storage_type = basic_sparse_set<Entity, Allocator>;

  template<typename Type>
  using storage_type = constness_as_t<storage<Entity, std::remove_const_t<Type>, memory::rebound_allocator_t<Allocator, std::remove_const_t<Type>>>, Type>;

  using entity_traits = ecs::entity_traits<Entity>;

public:

  using entity_type = entity_traits::value_type;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using iterator = registry_iterator<entity_type, entity_storage_type, free_list_type>;

  basic_registry() = default;

  basic_registry(const basic_registry&) = delete;

  basic_registry(basic_registry&& other) noexcept
  : _entities{std::move(other._entities)},
    _free_entities{std::move(other._free_entities)},
    _storages{std::move(other._storages)} { }

  ~basic_registry() {
    clear();
  }

  auto operator=(const basic_registry&) -> basic_registry& = delete;

  auto operator=(basic_registry&& other) noexcept -> basic_registry& {
    if (this != &other) {
      _entities = std::move(other._entities);
      _free_entities = std::move(other._free_entities);
      _storages = std::move(other._storages);
    }

    return *this;
  }

  auto begin() -> iterator {
    return iterator{_entities.begin(), _entities.end(), _free_entities};
  }

  auto end() -> iterator {
    return iterator{_entities.end(), _entities.end(), _free_entities};
  }

  auto clear() -> void {
    for (auto& [key, storage] : _storages) {
      storage->clear();
    }

    _entities.clear();
    _free_entities.clear();
  }

  auto create_entity() -> entity_type {
    if (!_free_entities.empty()) {
      auto index = *_free_entities.begin();
      _free_entities.erase(_free_entities.begin());

      return _entities.at(index);
    }

    const auto id = static_cast<entity_traits::entity_type>(_entities.size());

    auto new_entity = entity_traits::construct(id);

    _entities.push_back(new_entity);
    return new_entity;
  }

  auto destroy_entity(const entity_type& entity) -> void {
    // [NOTE] 2023-03-20 19:43 : Clear out all components that are owned by this entity
    for (auto& [type, storage] : _storages) {
      storage->remove(entity);
    }

    auto index = static_cast<std::size_t>(entity_traits::to_entity(entity));
    _free_entities.insert(index);
    _entities.at(index) = entity_traits::next(_entities.at(index));
  }

  auto is_valid_entity(const entity_type& entity) const noexcept -> bool {
    auto index = static_cast<std::size_t>(entity_traits::to_entity(entity));
    return index < _entities.size() && entity == _entities.at(index);
  }

  template<typename Component>
  auto has_component(const entity_type& entity) const -> bool {
    if (const auto storage = _try_get_storage<std::remove_const_t<Component>>(); storage) {
      return storage->contains(entity);
    }

    return false;
  }

  template<typename Component, typename... Args>
  auto add_component(const entity_type& entity, Args&&... args) -> Component& {
    auto& storage = _get_or_create_storage<std::remove_const_t<Component>>();

    return storage.add(entity, std::forward<Args>(args)...);
  }

  template<typename Component>
  auto remove_component(const entity_type& entity) -> bool {
    if (auto storage = _try_get_storage<std::remove_const_t<Component>>(); storage) {
      return storage->remove(entity);
    }

    return false;
  }

  /**
   * @brief Gets the component assigned to an entity
   * 
   * @tparam Component Type of the component
   * @param entity The entity the component is assigned to
   * 
   * @throws std::runtime_error when the entity does not have a component of the given type assigned to itself 
   * 
   * @return The component assigned to the entity
   */
  template<typename Component>
  auto get_component(const entity_type& entity) const -> const Component& {
    if (const auto component = try_get_component<std::remove_const_t<Component>>(entity); component) {
      return *component;
    }

    throw std::runtime_error{"Entity does not have component assigned to it"};
  }

  template<typename Component>
  auto get_component(const entity_type& entity) -> Component& {
    if (auto component = try_get_component<std::remove_const_t<Component>>(entity); component) {
      return *component;
    }

    throw std::runtime_error{"Entity does not have component assigned to it"};
  }

  template<typename Component>
  auto try_get_component(const entity_type& entity) const -> memory::observer_ptr<const Component> {
    if (const auto storage = _try_get_storage<std::remove_const_t<Component>>(); storage) {
      if (auto entry = storage->find(entity); entry != storage->cend()) {
        return memory::make_observer<const Component>(*entry);
      }
    }

    return nullptr;
  }

  template<typename Component>
  auto try_get_component(const entity_type& entity) -> memory::observer_ptr<Component> {
    if (auto storage = _try_get_storage<std::remove_const_t<Component>>(); storage) {
      if (auto entry = storage->find(entity); entry != storage->end()) {
        return memory::make_observer<Component>(*entry);
      }
    }

    return nullptr;
  }

  template<typename... Components>
  requires (variadic_template_size_v<Components...> != 0)
  auto create_view() -> basic_view<storage_type<Components>...> {
    return {_get_or_create_storage<std::remove_const_t<Components>>()...};
  }

  template<typename... Components>
  requires (variadic_template_size_v<Components...> != 0)
  auto create_view() const -> basic_view<storage_type<const Components>...> {
    return {_get_or_create_storage<std::remove_const_t<Components>>()...};
  }

private:

  template<typename Component>
  auto _get_or_create_storage() const -> const storage_type<Component>& {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _storages.find(type); entry != _storages.cend()) {
      return static_cast<const storage_type<Component>&>(*entry->second);
    }

    // [Note]: We use an empty storage placeholder in const context until we find it in the available storages
    static const auto placeholder = storage_type<Component>{};

    return placeholder;
  }

  template<typename Component>
  auto _get_or_create_storage() -> storage_type<Component>& {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _storages.find(type); entry != _storages.end()) {
      return static_cast<storage_type<Component>&>(*entry->second);
    }

    auto entry = _storages.insert({type, std::make_unique<storage_type<Component>>()}).first;

    return static_cast<storage_type<Component>&>(*entry->second);
  }

  template<typename Component>
  auto _try_get_storage() const -> memory::observer_ptr<const storage_type<Component>> {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _storages.find(type); entry != _storages.cend()) {
      return memory::make_observer<const storage_type<Component>>(static_cast<const storage_type<Component>*>(entry->second.get()));
    }

    return nullptr;
  }

  template<typename Component>
  auto _try_get_storage() -> memory::observer_ptr<storage_type<Component>> {
    const auto type = std::type_index{typeid(Component)};

    if (auto entry = _storages.find(type); entry != _storages.end()) {
      return memory::make_observer<storage_type<Component>>(static_cast<storage_type<Component>*>(entry->second.get()));
    }

    return nullptr;
  }

  entity_storage_type _entities;
  free_list_type _free_entities;

  std::unordered_map<std::type_index, std::unique_ptr<basic_storage_type>> _storages;

}; // class basic_registry

using registry = basic_registry<entity>;

} // namespace sbx::ecs

#endif // LIBSBX_REGISTRY_HPP_
