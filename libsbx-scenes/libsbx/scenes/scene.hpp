#ifndef LIBSBX_SCENES_SCENE_HPP_
#define LIBSBX_SCENES_SCENE_HPP_

#include <unordered_map>
#include <memory>
#include <utility>
#include <ranges>
#include <vector>
#include <typeindex>
#include <filesystem>

#include <range/v3/all.hpp>

#include <yaml-cpp/yaml.h>

#include <libsbx/ecs/registry.hpp>
#include <libsbx/ecs/entity.hpp>

#include <libsbx/math/transform.hpp>
#include <libsbx/math/uuid.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/components/directional_light.hpp>

namespace sbx::scenes {

class scene {

  friend class node;

  using signal_container = std::unordered_map<std::type_index, signals::signal<node&>>;

public:

  scene();

  auto create_child_node(node& parent, const std::string& tag = "", const math::transform& transform = math::transform{}) -> node;

  auto create_node(const std::string& tag = "", const math::transform& transform = math::transform{}) -> node;  

  auto destroy_node(const node& node) -> void;

  auto camera() -> node {
    return _camera;
  }

  auto world_transform(const node& node) -> math::matrix4x4;

  template<typename... Components>
  auto query() -> std::vector<node> {
    auto view = _registry.create_view<Components...>();
     
    return view 
      | ranges::views::transform([&](auto& entity) { return node{&_registry, entity}; })
      | ranges::to<std::vector>();
  }

  auto light() -> directional_light& {
    return _light;
  }

  auto find_node(const math::uuid& id) -> std::optional<node> {
    if (auto entry = _nodes.find(id); entry != _nodes.end()) {
      return entry->second;
    } 
      
    return std::nullopt;
  }

private:

  template<typename Component, typename... Args>
  auto _add_or_update_component(node& node, Args&&... args) -> void {
    if (node.has_component<Component>()) {
      auto& component = node.get_component<Component>();
      component = Component{std::forward<Args>(args)...};
    } else {
      node.add_component<Component>(std::forward<Args>(args)...);
    }
  }

  std::unordered_map<math::uuid, node> _nodes;

  ecs::registry _registry;
  node _root;
  node _camera;

  directional_light _light;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
