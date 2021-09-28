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

private:
  template<typename Component>
  [[nodiscard]] storage_type<Component>* _assure() const {
    static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types not allowed");
    const auto index = type_index<Component>::value();

    if(!(index < _pools.size())) {
      _pools.resize(size_type(index)+1u);
    }

    if(auto &&pool_data = _pools[index]; !pool_data.pool) {
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

  std::vector<pool_data> _pools{};
  std::vector<entity_type> _entities{};
  entity_type _free_list{tombstone};

}; // class basic_registry

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
