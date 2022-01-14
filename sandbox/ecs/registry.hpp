#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <type_traits>
#include <memory>
#include <vector>
#include <algorithm>

#include <types/type_traits.hpp>
#include <types/iterator_traits.hpp>

#include <utils/type_id.hpp>

#include "component.hpp"
#include "entity.hpp"
#include "storage.hpp"
#include "view.hpp"

namespace sbx {

template<typename>
class basic_registry;


using registry = basic_registry<entity>;


template<typename Entity>
class basic_registry {

  using entity_traits = sbx::entity_traits<Entity>;
  using basic_common_type = basic_sparse_set<Entity>;

  template<typename Component>
  using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type, Component>;

  using component_pool = std::unique_ptr<basic_common_type>;

public:

  using entity_type = Entity;
  using version_type = typename entity_traits::version_type;
  using size_type = std::size_t;

  basic_registry()
  : _pools{}, 
    _entities{},
    _free_list{tombstone_entity} { }

  basic_registry(const basic_registry&) = delete;

  basic_registry(basic_registry&&) = default;

  basic_registry& operator=(const basic_registry&) = delete;

  basic_registry& operator=(basic_registry&&) = default;

  template<typename Component>
  void prepare() {
    static_cast<void>(_assure<Component>());
  }

  template<typename Component>
  [[nodiscard]] size_type size() const noexcept { 
    const auto* pool = _pool_if_exists<Component>();
    return pool ? pool->size() : size_type{};
  }

  [[nodiscard]] size_type size() const noexcept {
    return _entities.size();
  }

  [[nodiscard]] size_type alive() const {
    auto size = _entities.size();

    for (auto current = _free_list; current != null_entity; --size) {
      current = _entities[entity_traits::to_entity(current)];
    } 

    return size;
  }

  template<typename... Components>
  void reserve(const size_type capacity) {
    if constexpr (sizeof...(Components) == 0) {
      _entities.reserve(capacity);
    } else {
      (_assure<Components>()->reserve(capacity), ...);
    }
  }

  template<typename Component>
  [[nodiscard]] size_type capacity() const {
    const auto* pool = _pool_if_exists<Component>();
    return pool ? pool->capacity() : size_type{};
  }

  [[nodiscard]] size_type capacity() const noexcept {
    return _entities.capacity();
  }

  template<typename... Components>
  void shrink_to_fit() {
    (_assure<Components>()->shrink_to_fit(), ...);
  }

  template<typename... Components>
  [[nodiscard]] bool is_empty() const {
    if constexpr (sizeof...(Components) == 0) {
      return !alive();
    } else {
      return [](const auto*... pool) { return ((!pool || pool->is_empty()) && ...); }(_pool_if_exists<Components>()...);
    }
  }

  [[nodiscard]] const entity_type* data() const noexcept {
    return _entities.data();
  }

  [[nodiscard]] entity_type released() const noexcept {
    return _free_list;
  }

  [[nodiscard]] bool is_valid(const entity_type entity) const {
    const auto position = size_type(entity_traits::to_entity(entity));
    return (position < _entities.size() && _entities[position] == entity);
  }

  [[nodiscard]] version_type current_version(const entity_type entity) const {
    const auto position = size_type(entity_traits::to_entity(entity));
    return entity_traits::to_version(position < _entities.size() ? _entities[position] : tombstone_entity);
  }

  [[nodiscard]] entity_type create_entity() {
    return (_free_list != null_entity) ? _recycle_identifier() : _entities.emplace_back(_generate_identifier(_entities.size()));
  }

  [[nodiscard]] entity_type create_entity(const entity_type hint) {
    const auto length = _entities.size();

    if (hint == null_entity || hint == tombstone_entity) {
      return create_entity();
    } else if (const auto requested = entity_traits::to_entity(hint); !(requested < length)) {
      _entities.resize(size_type(requested) + 1u, null_entity);

      for (auto position = length; position < requested; ++position) {
        _release_entity(generate_identifier(position), version_type{});
      }

      return (_entities[requested] = hint);
    } else if (const auto current = entity_traits::to_entity(_entities[requested]); requested == current) {
        return create_entity();
    } else {
        auto* iterator = &_free_list;
        for (; entity_traits::to_entity(*iterator) != requested; iterator = &_entities[entity_traits::to_entity(*iterator)]) {}
        *iterator = entity_traits::combine(current, entity_traits::to_integral(*iterator));
        return (_entities[requested] = hint);
    }
  }

  template<typename Iterator>
  void create_entity(Iterator first, Iterator last) {
    for (; _free_list != null_entity && first != last; ++first) {
      *first = _recycle_identifier();
    }

    const auto length = _entities.size();
    _entities.resize(length + std::distance(first, last), null_entity);

    for (auto position = length; first != last; ++first, ++position) {
      _entities[position] = _generate_identifier(position);
      *first = _entities[position];
    }
  }

  version_type release_entity(const entity_type entity) {
    return release_entity(entity, entity_traits::to_version(entity) + 1u);
  }

  version_type release_entity(const entity_type entity, const version_type version) {
    assert(orphan(entity));
    return _release_entity(entity, version);
  }

  template<typename Iterator>
  void release_entity(Iterator first, Iterator last) {
    for (; first != last; ++first) {
      release_entity(*first, entity_traits::to_version(*first) + 1u);
    }
  }

  version_type destroy_entity(const entity_type entity) {
    return destroy_entity(entity, entity_traits::to_version(entity) + version_type{1});
  }

  version_type destroy_entity(const entity_type entity, const version_type version) {
    assert(is_valid(entity));

    for (auto& pool : _pools) {
      pool && pool->remove(entity);
    }

    return _release_entity(entity, version);
  }

  template<typename Iterator>
  void destroy_entity(Iterator first, Iterator last) {
    if constexpr (is_iterator_type_v<typename basic_common_type::iterator, Iterator>) {
      for (; first != last; ++first) {
        destroy_entity(*first, entity_traits::to_version(*first) + 1u);
      }
    } else {
      for (auto& pool : _pools) {
        pool && pool->remove(first, last);
      }

      release_entity(first, last);
    }
  }

  template<typename Component, typename... Args>
  decltype(auto) emplace_component(const entity_type entity, Args&&... args) {
    assert(is_valid(entity));
    return _assure<Component>()->emplace(entity, std::forward<Args>(args)...);
  }

  template<typename Component, typename Iterator>
  void insert_component(Iterator first, Iterator last, const Component& value = {}) {
    assert(std::all_of(first, last, [this](const auto entity) { return is_valid(entity); }));
    _assure<Component>()->insert(first, last, value);
  }

  template<typename Component, typename... Args>
  decltype(auto) emplace_or_replace_component(const entity_type entity, Args&&... args) {
    assert(is_valid(entity));
    auto* pool = _assure<Component>();

    return pool->contains(entity)
      ? pool->patch(entity, [&args...](auto&... current) { return (( current = Component{std::forward<Args>(args)...}), ...); })
      : pool->emplace(entity, std::forward<Args>(args)...);
  }

  template<typename Component, typename... Functions>
  decltype(auto) patch_component(const entity_type entity, Functions&&... functions) {
    assert(is_valid(entity));
    return _assure<Component>()->patch(entity, std::forward<Functions>(functions)...);
  }

  template<typename Component, typename... Args>
  decltype(auto) replace_component(const entity_type entity, Args&&... args) {
    return _assure<Component>()->patch(entity, [&args...](auto&... current) { ((current = Component{std::forward<Args>(args)...}), ...); });
  }

  template<typename... Components>
  size_type remove_components(const entity_type entity) {
    assert(is_valid(entity));
    static_assert(sizeof...(Components) > 0, "Provide one or more component types");
    // [NOTE] KAJ 2021-11-03 13:51 - Maybe remove counter??
    return (_assure<Components>()->remove(entity) + ... + size_type{});
  }

  template<typename... Components, typename Iterator>
  size_type remove_components(Iterator first, Iterator last) {
    static_assert(sizeof...(Components) > 0, "Provide one or more component types");
    const auto pools = std::make_tuple(_assure<Components>()...);
    auto count = size_type{};

    for (; first != last; ++first) {
      const auto entity = *first;
      assert(is_valid(entity));
      count += (std::get<storage_type<Components>*>(pools)->remove(entity) + ...);
    }

    return count;
  }

  template<typename... Components>
  void erase_components(const entity_type entity) {
    assert(is_valid(entity));
    static_assert(sizeof...(Components) > 0, "Provide one or more component types");
    (_assure<Components>()->erase(entity), ...);
  }

  template<typename... Components, typename Iterator>
  void erase_components(Iterator first, Iterator last) {
    static_assert(sizeof...(Components) > 0, "Provide one or more component types");
    const auto pools = std::make_tuple(_assure<Components>()...);

    for (; first != last; ++first) {
      const auto entity = *first;
      assert(is_valid(entity));
      (std::get<storage_type<Components>*>(pools)->erase(entity), ...);
    }
  }

  template<typename... Components>
  void compact() {
    if constexpr (sizeof...(Components) == 0) {
      for (auto& pool : _pools) {
        pool && pool->compact();
      }
    } else {
      (_assure<Components>()->compact(), ...);
    }
  }

  template<typename... Components>
  [[nodiscard]] bool has_all_of(const entity_type entity) const {
    assert(is_valid(entity));
    const auto all_of = [entity](const auto*... pool) { return ((pool && pool->contains(entity)) && ...); };
    return all_of(_pool_if_exists<Components>()...);
  }

  template<typename... Components>
  [[nodiscard]] bool has_any_of(const entity_type entity) const {
    assert(is_valid(entity));
    const auto any_of = [entity](const auto*... pool) { return !((!pool || !pool->contains(entity)) && ...); }; 
    return any_of(_pool_if_exists<Components>()...);
  }

  template<typename... Components>
  [[nodiscard]] decltype(auto) get_components([[maybe_unused]] const entity_type entity) const {
    assert(is_valid(entity));

    if constexpr (sizeof...(Components) == 1) {
      const auto* pool = _pool_if_exists<std::remove_const_t<Components>...>();
      assert(pool);
      return pool->get(entity);
    } else {
      return std::forward_as_tuple(get_components<Components>(entity)...);
    }
  }

  template<typename... Components>
  [[nodiscard]] decltype(auto) get_components([[maybe_unused]] const entity_type entity) {
    assert(is_valid(entity));

    if constexpr (sizeof...(Components) == 1) {
      return (const_cast<Components&>(_assure<std::remove_const_t<Components>>()->get(entity)), ...);
    } else {
      return std::forward_as_tuple(get_components<Components>(entity)...);
    }
  }

  template<typename Component, typename... Args>
  [[nodiscard]] decltype(auto) get_or_emplace_components(const entity_type entity, Args&&... args) {
    assert(is_valid(entity));
    auto* pool = _assure<Component>();
    return pool->contains(entity) ? pool->get(entity) : pool->emplace(entity, std::forward<Args>(args)...);
  }

  template<typename... Components>
  [[nodiscard]] auto try_get_components([[maybe_unused]] const entity_type entity) const {
    assert(is_valid(entity));

    if constexpr (sizeof...(Components) == 1) {
      const auto* pool = _pool_if_exists<std::remove_const_t<Components>...>();
      return (pool && pool->contains(entity)) ? &pool->get(entity) : nullptr;
    } else {
      return std::make_tuple(try_get_components<Components>(entity)...);
    }
  }

  template<typename... Components>
  [[nodiscard]] auto try_get_components([[maybe_unused]] const entity_type entity) {
    assert(is_valid(entity));

    if constexpr (sizeof...(Components) == 1) {
      return (const_cast<Components*>(std::as_const(*this).template try_get_components<Components>(entity)), ...);
    } else {
      return std::make_tuple(try_get_components<Components>(entity)...);
    }
  }

  template<typename... Components>
  void clear() {
    if constexpr (sizeof...(Components) == 0) {
      for (auto& pool : _pools) {
        pool && (pool->clear(), true);
      }

      each([this](const auto entity) { _release_entity(entity, entity_traits::to_version(entity) + version_type{1}); });
    } else {
      (_assure<Components>()->clear(), ...);
    }
  }

  template<typename Function>
  void each(Function function) const {
    if (_free_list == null_entity) {
      for (auto position = _entities.size(); position; --position) {
        function(_entities[position - 1]);
      }
    } else {
      for (auto position = _entities.size(); position; --position) {
        if (const auto entity = _entities[position - 1]; entity_traits::to_entity(entity) == (position - 1)) {
          function(entity);
        }
      }
    }
  }

  [[nodiscard]] bool orphan(const entity_type entity) const {
    assert(is_valid(entity));
    return std::none_of(_pools.cbegin(), _pools.cend(), [entity](auto& pool) { return pool && pool->contains(entity); });
  }

  template<typename... Components, typename... Excludes>
  [[nodiscard]] basic_view<entity_type, get_t<std::add_const_t<Components>...>, exclude_t<Excludes...>> view(exclude_t<Excludes...> = {}) const {
    static_assert(sizeof...(Components) > 0, "Exclusion-only views are not supported");
    return {*_assure<std::remove_const_t<Components>>()..., *_assure<Excludes>()...};
  }

  template<typename... Components, typename... Excludes>
  [[nodiscard]] basic_view<entity_type, get_t<Components...>, exclude_t<Excludes...>> view(exclude_t<Excludes...> = {}) {
    static_assert(sizeof...(Components) > 0, "Exclusion-only views are not supported");
    return {*_assure<std::remove_const_t<Components>>()..., *_assure<Excludes>()...};
  }

private:

  template<typename Component>
  [[nodiscard]] storage_type<Component>* _assure() const {
    static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types not allowed");

    constexpr auto index = type_id<Component>{};

    if (index >= _pools.size()) {
      _pools.resize(size_type(index) + 1u);
    }

    if (auto& pool = _pools[index]; !pool) {
        pool.reset(new storage_type<Component>());
    }

    return static_cast<storage_type<Component>*>(_pools[index].get());
  }

  template<typename Component>
  [[nodiscard]] const storage_type<Component>* _pool_if_exists() const noexcept {
    static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types not allowed");
    constexpr auto index = type_id<Component>{};

    return (index >= _pools.size() || !_pools[index]) ? nullptr : static_cast<const storage_type<Component>*>(_pools[index].get());
  }

  auto _generate_identifier(const std::size_t position) noexcept {
    assert(position < entity_traits::to_integral(null_entity));
    
    return entity_traits::combine(static_cast<typename entity_traits::entity_type>(position), {});
  }

  auto _recycle_identifier() noexcept {
      assert(_free_list != null_entity);
      const auto current = entity_traits::to_entity(_free_list);

      _free_list = entity_traits::combine(entity_traits::to_integral(_entities[current]), tombstone_entity);

      return (_entities[current] = entity_traits::combine(current, entity_traits::to_integral(_entities[current])));
  }

  auto _release_entity(const Entity entity, const typename entity_traits::version_type version) {
    // const typename entity_traits::version_type new_version = version + (version == entity_traits::to_version(tombstone_entity));
    const auto new_version = static_cast<version_type>(version + (version == entity_traits::to_version(tombstone_entity)));

    _entities[entity_traits::to_entity(entity)] = entity_traits::construct(entity_traits::to_integral(_free_list), new_version);
    _free_list = entity_traits::combine(entity_traits::to_integral(entity), tombstone_entity);

    return new_version;
  }

  mutable std::vector<component_pool> _pools{};
  std::vector<entity_type> _entities{};
  entity_type _free_list{tombstone_entity};

}; // class basic_registry

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
