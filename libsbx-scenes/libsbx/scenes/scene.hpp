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

#include <libsbx/memory/octtree.hpp>

#include <libsbx/ecs/registry.hpp>
#include <libsbx/ecs/entity.hpp>

#include <libsbx/math/transform.hpp>
#include <libsbx/math/uuid.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/components/directional_light.hpp>

namespace sbx::scenes {

class scene {

  friend class node;

public:

  scene(const std::filesystem::path& path);

  virtual ~scene() = default;

  auto create_child_node(node& parent, const std::string& tag = "", const math::transform& transform = math::transform{}) -> node;

  auto create_node(const std::string& tag = "", const math::transform& transform = math::transform{}) -> node;  

  auto destroy_node(const node& node) -> void;

  auto camera() -> node {
    return _camera;
  }

  auto world_transform(const node& node) -> math::matrix4x4;

  auto world_position(const node& node) -> math::vector3;

  template<typename... Components>
  auto query() -> std::vector<node> {
    auto view = _registry.view<Components...>();

    return view | ranges::views::transform([&](auto& entity) { return node{&_registry, entity}; }) | ranges::to<std::vector>();
  }

  auto light() -> directional_light& {
    return _light;
  }

  auto root() -> node {
    return _root;
  }

  auto light_space() -> math::matrix4x4 {
    const auto position = _light.direction() * -20.0f;

    const auto view = math::matrix4x4::look_at(position, position + _light.direction() * 20.0f, math::vector3::up);
    const auto projection = math::matrix4x4::orthographic(-40.0f, 40.0f, -40.0f, 40.0f, 0.1f, 300.0f);

    return projection * view;
  }

  auto find_node(const math::uuid& id) -> std::optional<node> {
    if (auto entry = _nodes.find(id); entry != _nodes.end()) {
      return entry->second;
    } 
      
    return std::nullopt;
  }

private:

  auto _load_assets(const YAML::Node& assets) -> void;
  auto _load_nodes(const YAML::Node& nodes) -> void;

  std::unordered_map<math::uuid, node> _nodes;

  ecs::registry _registry;
  node _root;
  node _camera;

  memory::octree<math::uuid> _octree;

  directional_light _light;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
