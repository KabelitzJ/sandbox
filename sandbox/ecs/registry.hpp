#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <vector>
#include <memory>

#include "entity.hpp"
#include "type_index.hpp"
#include "sparse_set.hpp"
#include "component.hpp"
#include "storage.hpp"

namespace sbx {

template<typename Entity>
class basic_registry {

  using entity_traits = sbx::entity_traits<Entity>;
  using basic_common_type = basic_sparse_set<Entity>;

  template<typename Component>
  using storage_type = constness_as_t<Component, basic_storage<Entity, std::remove_const_t<Component>>>;

  struct pool_data {
    std::unique_ptr<basic_common_type> pool{};
  }; // struct pool_data

public:
  using entity_type = Entity;
  using version_type = typename entity_traits::version_type;
  using size_type = std::size_t;

  basic_registry() = default;

  template<typename Component>
  void prepare() {
    static_cast<void>(_assure<Component>());
  }

  template<typename Component>
  [[nodiscard]] size_type size() const {
    const auto* component_pool = _pool_if_exists<Component>();

    return component_pool ? component_pool->size() : size_type{};
  }

  [[nodiscard]] size_type size() const noexcept {
    return _entities.size();
  }

  [[nodiscard]] size_type alive() const {
    auto size = _entities.size();

    for(auto current = _free_list; current != null; --size) {
      current = _entities[entity_traits::to_entity(current)];
    }

    return size;
  }

  template<typename... Component>
  void reserve(const size_type cap) {
    if constexpr(sizeof...(Component) == 0) {
      _entities.reserve(cap);
    } else {
      (_assure<Component>()->reserve(cap), ...);
    }
  }

  template<typename Component>
  [[nodiscard]] size_type capacity() const {
      const auto* component_pool = _pool_if_exists<Component>();
      return component_pool ? component_pool->capacity() : size_type{};
  }

  [[nodiscard]] size_type capacity() const noexcept {
    return _entities.capacity();
  }

  template<typename... Component>
  void shrink_to_fit() {
    (_assure<Component>()->shrink_to_fit(), ...);
  }

  template<typename... Component>
  [[nodiscard]] bool empty() const {
    if constexpr(sizeof...(Component) == 0) {
      return !alive();
    } else {
      return [](const auto* ... component_pool) { return ((!component_pool || component_pool->empty()) && ...); }(_pool_if_exists<Component>()...);
    }
  }

  [[nodiscard]] const entity_type* data() const noexcept {
    return _entities.data();
  }

  [[nodiscard]] bool is_valid(const entity_type entity) const {
    const auto position = size_type(entity_traits::to_entity(entity));

    return (position < _entities.size() && _entities[position] == entity);
  }

  [[nodiscard]] version_type current_version(const entity_type entity) const {
    const auto position = size_type(entity_traits::to_entity(entity));

    return entity_traits::to_version(position < _entities.size() ? _entities[position] : tombstone);
  }

  [[nodiscard]] entity_type create() {
    return (_free_list == null) ? _entities.emplace_back(_generate_identifier(_entities.size())) : _recycle_identifier();
  }

  [[nodiscard]] entity_type create(const entity_type hint) {
    const auto length = _entities.size();

    if(hint == null || hint == tombstone) {
      return create();
    } else if(const auto requested = entity_traits::to_entity(hint); !(requested < length)) {
      _entities.resize(size_type(requested) + 1u, null);

      for(auto position = length; position < requested; ++position) {
        release_entity(_generate_identifier(position), {});
      }

      return (_entities[requested] = hint);
    } else if(const auto current = entity_traits::to_entity(_entities[requested]); requested == current) {
      return create();
    } else {
      auto *it = &_free_list;
      
      for(; entity_traits::to_entity(*it) != requested; it = &_entities[entity_traits::to_entity(*it)]);
      *it = entity_traits::combine(current, entity_traits::to_integral(*it));
      
      return (_entities[requested] = hint);
    }
  }

  version_type destroy(const entity_type entity) {
    return destroy(entity, entity_traits::to_version(entity) + 1u);
  }


  version_type destroy(const entity_type entity, const version_type version) {
    assert(is_valid(entity));

    for(auto&& pool_data : _pools) {
        pool_data.pool && pool_data.pool->remove(entity, this);
    }

    return _release_entity(entity, version);
  }

  template<typename Component, typename... Args>
  decltype(auto) emplace(const entity_type entity, Args&&... args) {
    assert(is_valid(entity));

    return _assure<Component>()->emplace(entity, std::forward<Args>(args)...);
  }

  template<typename... Component>
  size_type remove(const entity_type entity) {
    static_assert(sizeof...(Component) > 0, "Provide one or more component types");
    assert(is_valid(entity));

    return (_assure<Component>()->remove(entity, this) + ... + size_type{});
  }

  template<typename... Component>
  void compact() {
    if constexpr(sizeof...(Component) == 0) {
      for(auto&& pool_data: _pools) {
        pool_data.pool && (pool_data.pool->compact(), true);
      }
    } else {
      (_assure<Component>()->compact(), ...);
    }
  }

  template<typename... Component>
  [[nodiscard]] bool all_of(const entity_type entity) const {
    assert(is_valid(entity));
    return [entity](const auto* ... component_pool) { return ((component_pool && component_pool->contains(entity)) && ...); }(_pool_if_exists<Component>()...);
  }

  template<typename... Component>
  [[nodiscard]] bool any_of(const entity_type entity) const {
    assert(is_valid(entity));
    return [entity](const auto* ... component_pool) { return !((!component_pool || !component_pool->contains(entity)) && ...); }(_pool_if_exists<Component>()...);
  }

  template<typename... Component>
  [[nodiscard]] decltype(auto) get([[maybe_unused]] const entity_type entity) const {
    assert(is_valid(entity));

    if constexpr(sizeof...(Component) == 1) {
      const auto* component_pool = _pool_if_exists<std::remove_const_t<Component>...>();
      assert(component_pool);
      return component_pool->get(entity);
    } else {
      return std::forward_as_tuple(get<Component>(entity)...);
    }
  }

  template<typename... Component>
  [[nodiscard]] decltype(auto) get([[maybe_unused]] const entity_type entity) {
    assert(is_valid(entity));

    if constexpr(sizeof...(Component) == 1) {
      return (const_cast<Component &>(_assure<std::remove_const_t<Component>>()->get(entity)), ...);
    } else {
      return std::forward_as_tuple(get<Component>(entity)...);
    }
  }

  template<typename... Component>
  void clear() {
    if constexpr(sizeof...(Component) == 0) {
      for(auto&& pool_data: _pools) {
        pool_data.pool && (pool_data.pool->clear(this), true);
      }

      each([this](const auto entity) { release_entity(entity, entity_traits::to_version(entity) + 1u); });
    } else {
      (_assure<Component>()->clear(this), ...);
    }
  }

  template<typename Func>
  void each(Func func) const {
    if(_free_list == null) {
      for(auto position = _entities.size(); position; --position) {
        func(_entities[position - 1]);
      }
    } else {
      for(auto position = _entities.size(); position; --position) {
        if(const auto entity = _entities[position - 1]; entity_traits::to_entity(entity) == (position - 1)) {
          func(entity);
        }
      }
    }
  }

  // [Note] KAJ 28.09.2021 13:04: Add view functionalities

private:
  template<typename Component>
  [[nodiscard]] storage_type<Component>* _assure() const {
    static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types not allowed");
    const auto index = type_index<Component>::value();

    if(!(index < _pools.size())) {
      _pools.resize(size_type(index) + 1u);
    }

    if(auto&& pool_data = _pools[index]; !pool_data.pool) {
      pool_data.pool.reset(new storage_type<Component>());
    }

    return static_cast<storage_type<Component>*>(_pools[index].pool.get());
  }

  template<typename Component>
  [[nodiscard]] const storage_type<Component>* _pool_if_exists() const noexcept {
    static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types not allowed");
    
    const auto index = type_index<Component>::value();
    
    return (!(index < _pools.size()) || !_pools[index].pool) ? nullptr : static_cast<const storage_type<Component> *>(_pools[index].pool.get());
  }

  auto _generate_identifier(const std::size_t position) noexcept {
    assert(position < entity_traits::to_integral(null));

    return entity_traits::combine(static_cast<typename entity_traits::entity_type>(position), {});
  }

  auto _recycle_identifier() noexcept {
    assert(_free_list != null);

    const auto current = entity_traits::to_entity(_free_list);
    _free_list = entity_traits::combine(entity_traits::to_integral(_entities[current]), tombstone);
    return (_entities[current] = entity_traits::combine(current, entity_traits::to_integral(_entities[current])));
  }

  auto _release_entity(const Entity entity, const version_type version) {
    const version_type next_version = version + (version == entity_traits::to_version(tombstone));
    _entities[entity_traits::to_entity(entity)] = entity_traits::construct(entity_traits::to_integral(_free_list), next_version);
    _free_list = entity_traits::combine(entity_traits::to_integral(entity), tombstone);

    return next_version;
  }

  mutable std::vector<pool_data> _pools{};
  std::vector<entity_type> _entities{};
  entity_type _free_list{tombstone};

}; // class basic_registry

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
