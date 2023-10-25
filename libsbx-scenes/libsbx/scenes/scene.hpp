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

#include <libsbx/ecs/registry.hpp>
#include <libsbx/ecs/entity.hpp>

#include <libsbx/math/transform.hpp>
#include <libsbx/math/uuid.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/scenes/node.hpp>

namespace sbx::scenes {

class scene {

  friend class node;

  using signal_container = std::unordered_map<std::type_index, signals::signal<node&>>;

public:

  scene();

  scene(const std::filesystem::path& path)
  : scene{} {
    
    auto node = YAML::LoadFile(path.string());

    const auto name = node["name"].as<std::string>();

    core::logger::debug("Scene name: {}", name);

    const auto entities = node["entities"].as<std::vector<YAML::Node>>();

    for (const auto& entity : entities) {
      const auto entity_name = entity["name"].as<std::string>();

      core::logger::debug("  Entity name: {}", entity_name);

      const auto components = entity["components"].as<std::vector<YAML::Node>>();

      for (const auto& component : components) {
        const auto component_type = component["type"].as<std::string>();

        core::logger::debug("    Component type: {}", component_type);
      }
    }
  }

  auto start() -> void;

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

    auto to_node = std::views::transform([&](auto& entity) { return node{&_registry, entity, &_on_component_added, &_on_component_removed}; });

    return view | to_node | ranges::to<std::vector>();
  }

  template<typename Component>
  auto on_component_added() -> signals::signal<node&>& {
    const auto type = std::type_index{typeid(Component)};

    return _on_component_added[type];
  }

  template<typename Component>
  auto on_component_removed() -> signals::signal<node&>& {
    const auto type = std::type_index{typeid(Component)};

    return _on_component_removed[type];
  }

private:

  signal_container _on_component_added;
  signal_container _on_component_removed;

  std::unordered_map<math::uuid, node> _nodes;

  ecs::registry _registry;
  node _root;
  node _camera;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
