#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <type_traits>
#include <memory>
#include <vector>

#include <types/type_traits.hpp>

#include <util/type_id.hpp>

#include "entity.hpp"
#include "component.hpp"
#include "storage.hpp"

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

protected:

private:
  template<typename Component>
  [[nodiscard]] storage_type<Component>* _assure() const {
    static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types not allowed");

    constexpr auto index = type_id<Component>{};

    if(index >= _pools.size()) {
      _pools.resize(size_type(index) + 1u);
    }

    if(auto& pool = _pools[index]; !pool) {
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
    const typename entity_traits::version_type new_version = version + (version == entity_traits::to_version(tombstone_entity));

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
