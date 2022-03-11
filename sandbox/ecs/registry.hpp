#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>

#include <container/dynamic_bitset.hpp>

#include "entity.hpp"

namespace sbx {

class registry {

  using component_handle = std::unique_ptr<void, void(*)(void*)>;
  using component_container = std::unordered_map<entity::value_type, component_handle>;

public:

  using size_type = std::size_t;

  registry() = default;

  registry(const registry&) = delete;

  registry(registry&&) noexcept;

  ~registry() = default;

  registry& operator=(const registry&) = delete;

  registry& operator=(registry&&) noexcept;

  [[nodiscard]] entity create_entity();

  void destroy_entity(const entity& entity);

  [[nodiscard]] bool is_valid_entity(const entity& entity) const noexcept;

  template<typename Component, typename... Args>
  requires (std::is_constructible_v<Component, Args...>)
  Component& add_component(const entity& entity, Args&&... args) {
    if (!is_valid_entity(entity)) {
      throw std::invalid_argument("entity is invalid");
    }

    const auto id = _component_id<Component>();

    auto& container = _components[id];

    auto deleter = [](auto* pointer){ delete static_cast<Component*>(pointer); };
    auto component = component_handle{new Component{std::forward<Args>(args)...}, deleter};

    container.emplace(entity._value, std::move(component));

    const auto index = static_cast<size_type>(entity._id());

    _entities[index].signature.set(id);

    return *static_cast<Component*>(container[entity].get());
  }

  template<typename Component>
  void remove_component(const entity& entity) {
    if (!is_valid_entity(entity)) {
      throw std::invalid_argument("entity is invalid");
    }

    const auto id = _component_id<Component>();

    const auto index = static_cast<size_type>(entity._id());

    _entities[index].signature.reset(id);

    _components[id].erase(entity._value);
  }

  template<typename Component>
  [[nodiscard]] bool has_component(const entity& entity) const noexcept {
    if (!is_valid_entity(entity)) {
      return false;
    }

    const auto id = _component_id<Component>();

    const auto index = static_cast<size_type>(entity._id());

    return _entities[index].signature.test(id);
  }

  template<typename Component>
  [[nodiscard]] Component& get_component(const entity& entity) {
    if (!is_valid_entity(entity)) {
      throw std::invalid_argument("entity is invalid");
    }

    const auto id = _component_id<Component>();

    const auto index = static_cast<size_type>(entity._id());

    if (!_entities[index].signature.test(id)) {
      throw std::invalid_argument("entity does not have component");
    }

    auto& container = _components[id];

    return *static_cast<Component*>(container[entity].get());
  }

  template<typename... Component>
  void for_all() {

  }

private:

  struct entity_data {
    entity entity{};
    dynamic_bitset signature{};
  };

  template<typename Component>
  static size_type _component_id() noexcept {
    static auto id = size_type{0};
    return id++;
  }

  std::vector<entity_data> _entities{};
  std::queue<size_type> _free_entities{};
  std::unordered_map<size_type, component_container> _components{};

}; // class registry

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
