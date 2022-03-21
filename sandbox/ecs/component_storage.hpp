#ifndef SBX_ECS_COMPONENT_STORAGE_HPP_
#define SBX_ECS_COMPONENT_STORAGE_HPP_

#include <memory>
#include <vector>
#include <iostream>

#include <memory/pool_storage.hpp>

#include "entity_set.hpp"

namespace sbx {

template<standard_layout Type>
class component_storage : public entity_set {

  using base_type = entity_set;

  using pool_type = pool_storage<Type, 1024>;

public:

  using value_type = Type;
  using size_type = std::size_t;

  template<typename... Args>
  value_type& emplace(const entity& entity, Args&&... args) {
    if (contains(entity)) {
      throw std::runtime_error("entity already has component");
    }

    if constexpr (std::is_aggregate_v<value_type>) {
      const auto itr = _emplace(entity, value_type{std::forward<Args>(args)...});
      return *_components[static_cast<size_type>(itr.index())].get();
    } else {
      const auto itr = _emplace(entity, std::forward<Args>(args)...);
      return *_components[static_cast<size_type>(itr.index())].get();
    }
  }

private:

  struct component_deleter {

    component_deleter(pool_type* pool) : _pool{pool} { }

    void operator()(value_type* data) const {
      _pool->deallocate(data);
    }

  private:

    pool_type* _pool{};

  };

  template<typename... Args>
  base_type::iterator _emplace(const entity& entity, Args&&... args) {
    const auto itr = base_type::_try_emplace(entity);

    auto component = std::unique_ptr<value_type, component_deleter>(_pool.allocate(), component_deleter{std::addressof(_pool)});
    std::construct_at(component.get(), std::forward<Args>(args)...);

    _components.emplace_back(std::move(component));

    return itr;
  }

  base_type::iterator _try_emplace(const entity& entity) override {
    return _emplace(entity);
  }

  void _swap_and_pop(const entity& entity) override {
    std::swap(_components.back(), _components[base_type::index(entity)]);

    std::destroy_at(_components.back().get());
    _components.pop_back();
    
    base_type::_swap_and_pop(entity);
  }

  pool_type _pool{};
  std::vector<std::unique_ptr<value_type, component_deleter>> _components{};

}; // class component_storage

} // namespace sbx

#endif // SBX_ECS_COMPONENT_STORAGE_HPP_
