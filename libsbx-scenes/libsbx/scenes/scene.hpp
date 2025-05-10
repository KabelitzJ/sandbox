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
#include <libsbx/scenes/components/id.hpp>

namespace sbx::scenes {

class scene {

  // friend class node;

public:

  using node_type = node;
  using registry_type = ecs::basic_registry<node_type>;

  // template<typename... Get, typename... Exclude>
  // using query_result = ecs::basic_view

  // template<typename Type, typename... Other>
  // using const_query_result = registry_type::const_view_type<Type, Other...>;

  template<typename... Exclude>
  inline static constexpr auto query_filter = ecs::exclude<Exclude...>;

  scene(const std::filesystem::path& path);

  virtual ~scene() = default;

  auto create_child_node(const node_type parent, const std::string& tag = "", const math::transform& transform = math::transform{}) -> node_type;

  auto create_node(const std::string& tag = "", const math::transform& transform = math::transform{}) -> node_type;  

  auto destroy_node(const node_type node) -> void;

  auto camera() -> node {
    return _camera;
  }

  auto world_transform(const node_type node) -> math::matrix4x4;

  auto world_normal(const node_type node) -> math::matrix4x4;

  auto world_position(const node_type node) -> math::vector3;

  template<typename Type, typename... Other, typename... Exclude>
  auto query(ecs::exclude_t<Exclude...> = ecs::exclude_t{}) -> decltype(auto) {
    return _registry.view<Type, Other...>(ecs::exclude<Exclude...>);
  }

  template<typename Type, typename... Other, typename... Exclude>
  auto query(ecs::exclude_t<Exclude...> = ecs::exclude_t{}) const -> decltype(auto) {
    return _registry.view<Type, Other...>(ecs::exclude<Exclude...>);
  }

  template<typename Type, typename Compare, typename Sort = utility::std_sort, typename... Args>
  auto sort(Compare compare, Sort sort = Sort{}, Args&&... args) -> void {
    _registry.sort<Type>(std::move(compare), std::move(sort), std::forward<Args>(args)...);
  }

  template<typename Component>
  auto has_component(const node_type node) const -> bool {
    return _registry.all_of<Component>(node);
  }

  template<typename Component, typename... Args>
  auto add_component(const node_type node, Args&&... args) -> Component& {
    return _registry.emplace<Component>(node, std::forward<Args>(args)...);
  }

  template<typename Component>
  auto get_component(const node_type node) const -> const Component& {
    return _registry.get<Component>(node);
  }

  template<typename Component>
  auto get_component(const node_type node) -> Component& {
    return _registry.get<Component>(node);
  }

  template<typename Component, typename... Args>
  auto get_or_add_component(const node_type node, Args&&... args) -> Component& {
    return _registry.get_or_emplace(node, std::forward<Args>(args)...);
  }

  auto light() -> directional_light& {
    return _light;
  }

  auto root() -> node_type {
    return _root;
  }

  auto light_space() -> math::matrix4x4 {
    const auto position = _light.direction() * -20.0f;

    const auto view = math::matrix4x4::look_at(position, position + _light.direction() * 20.0f, math::vector3::up);
    const auto projection = math::matrix4x4::orthographic(-40.0f, 40.0f, -40.0f, 40.0f, 0.1f, 300.0f);

    return projection * view;
  }

  auto find_node(const scenes::id& id) -> node_type {
    if (auto entry = _nodes.find(id); entry != _nodes.end()) {
      return entry->second;
    } 
      
    return node_type::null;
  }

private:

  auto _load_assets(const YAML::Node& assets) -> void;
  auto _load_nodes(const YAML::Node& nodes) -> void;

  std::unordered_map<math::uuid, node_type> _nodes;

  registry_type _registry;
  node_type _root;
  node_type _camera;

  memory::octree<math::uuid> _octree;

  directional_light _light;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
