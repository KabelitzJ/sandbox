#ifndef SBX_CORE_SCENE_HPP_
#define SBX_CORE_SCENE_HPP_

#include <unordered_set>

#include <ecs/entity.hpp>
#include <ecs/registry.hpp>

namespace sbx {

class scene {

public:

  // [TODO] KAJ 2021-11-03 09:42 - Add a constructor that loads an existing scene
  scene();

  scene(const scene&) = delete;

  scene(scene&&) = default;

  ~scene();

  scene& operator=(const scene&) = delete;

  scene& operator=(scene&&) = default;

  entity create_entity(const entity parent = null_entity);

  void destroy_entity(const entity entity);

  template<typename Component, typename... Args>
  decltype(auto) emplace_component(const entity entity, Args&&... args) {
    return _registry.emplace_component<Component>(entity, std::forward<Args>(args)...);
  }

  template<typename... Components>
  decltype(auto) get_components(const entity entity) const {
    return _registry.get_components<Components...>(entity);
  }

  template<typename... Components>
  decltype(auto) get_components(const entity entity) {
    return _registry.get_components<Components...>(entity);
  }

  template<typename... Components>
  void remove_components(const entity entity) {
    return _registry.remove_components<Components...>(entity);
  }

  template<typename... Components>
  [[nodiscard]] bool has_components(const entity entity) const {
    return _registry.has_all_of<Components...>(entity);
  }

  template<typename... Components, typename... Excludes>
  [[nodiscard]] decltype(auto) create_view(exclude_t<Excludes...> = {}) const {
    return _registry.view<Components..., Excludes...>();
  }

  template<typename... Components, typename... Excludes>
  [[nodiscard]] decltype(auto) create_view(exclude_t<Excludes...> = {}) {
    return _registry.view<Components..., Excludes...>();
  }

private:

  registry _registry{};
  entity _root{null_entity};

}; // class scene

} // namespace sbx

#endif // SBX_CORE_SCENE_HPP_
