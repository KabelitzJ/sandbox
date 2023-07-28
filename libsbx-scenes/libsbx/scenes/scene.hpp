#ifndef LIBSBX_SCENES_SCENE_HPP_
#define LIBSBX_SCENES_SCENE_HPP_

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/math/uuid.hpp>

#include <libsbx/ecs/registry.hpp>
#include <libsbx/ecs/entity.hpp>

#include <libsbx/scenes/camera.hpp>

#include <libsbx/scenes/transform.hpp>

namespace sbx::scenes {

class scene {

public:

  class node {

    friend class scene;

  public:

    ~node() = default;

    template<typename Component>
    auto get_component() -> Component& {
      return _registry->get_component<Component>(_entity);
    }

    template<typename Component, typename... Args>
    auto add_component(Args&&... args) -> Component& {
      return _registry->add_component<Component, Args...>(_entity, std::forward<Args>(args)...);
    }

  private:

    node(memory::observer_ptr<ecs::registry> registry, ecs::entity entity)
    : _registry{registry}, 
      _entity{entity} { }

    memory::observer_ptr<ecs::registry> _registry;
    ecs::entity _entity;

  }; // class node

  scene();

  ~scene();

  auto create_node(const scenes::transform& transform = scenes::transform{}) -> node {
    auto entity = _registry.create_entity();

    auto node = scene::node{memory::make_observer<ecs::registry>(_registry), entity};

    auto uuid = _registry.add_component<math::uuid>(entity);
    _nodes.insert({uuid, node});

    _registry.add_component<scenes::transform>(entity, transform);

    return node;
  }

  auto camera() const -> const scenes::camera& {
    return *_camera;
  }

  template<typename... Args>
  auto set_camera(Args&&... args) -> void {
    _camera = std::make_unique<scenes::camera>(std::forward<Args>(args)...);
  }

  template<typename... Components>
  auto query(std::function<void(node&)> callback) -> void {
    auto view = _registry.create_view<Components...>();

    for (const auto& entity : view) {
      auto node = scene::node{memory::make_observer<ecs::registry>(_registry), entity};

      std::invoke(callback, node);
    }
  }

private:

  ecs::registry _registry;
  std::unordered_map<math::uuid, node> _nodes;

  std::unique_ptr<scenes::camera> _camera;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
