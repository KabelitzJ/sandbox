#ifndef SBX_ECS_COMPONENT_CONTAINER_HPP_
#define SBX_ECS_COMPONENT_CONTAINER_HPP_

#include <memory>
#include <vector>
#include <iostream>
#include <queue>

#include <memory/pool_allocator.hpp>

#include <meta/concepts.hpp>

#include <utils/noncopyable.hpp>

#include "entity_set.hpp"

namespace sbx {

template<typename Type>
concept component = !std::is_abstract_v<Type> && !std::is_const_v<Type> && !std::is_volatile_v<Type> && standard_layout<Type>;

template<component Type, allocator<Type> Allocator = pool_allocator<Type, 516>>
class component_container : public entity_set, public noncopyable {

  using base_type = entity_set;

  using allocator_type = Allocator;

  using allocator_traits = std::allocator_traits<allocator_type>;

  using component_storage_type = std::unique_ptr<Type, void(*)(Type*)>;

public:

  using value_type = Type;
  using size_type = std::size_t;

  component_container() = default;

  ~component_container() = default;

  template<typename... Args>
  value_type& emplace(const entity& entity, Args&&... args) {
    if (base_type::contains(entity)) {
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

  void erase(const entity& entity) {
    _swap_and_pop(entity);
  }

  const value_type& get(const entity& entity) const {
    if (!contains(entity)) {
      throw std::runtime_error("entity does not have component");
    }

    return *_components[base_type::_index(entity)].get();
  }

  value_type& get(const entity& entity) {
    return const_cast<value_type&>(std::as_const(*this).get(entity));
  }

private:

  template<typename... Args>
  base_type::iterator _emplace(const entity& entity, Args&&... args) {
    const auto itr = base_type::_try_emplace(entity);

    auto component_deleter = [this](auto* ptr){
      allocator_traits::destroy(_allocator, ptr);
      allocator_traits::deallocate(_allocator, ptr, 1);
    };

    auto component = component_storage_type{allocator_traits::allocate(_allocator, 1), component_deleter};
    std::construct_at(component.get(), std::forward<Args>(args)...);

    _components.emplace_back(std::move(component));

    return itr;
  }

  base_type::iterator _try_emplace(const entity& entity) override {
    return _emplace(entity);
  }

  void _swap_and_pop(const entity& entity) override {
    std::swap(_components.back(), _components[base_type::_index(entity)]);

    std::destroy_at(_components.back().get());
    _components.pop_back();
    
    base_type::_swap_and_pop(entity);
  }

  allocator_type _allocator{};
  std::vector<component_storage_type> _components{};

}; // class component_container

} // namespace sbx

#endif // SBX_ECS_COMPONENT_CONTAINER_HPP_
