#include <libsbx/scenes/scene.hpp>

#include <unordered_map>

// #include <portable-file-dialogs.h>

#include <yaml-cpp/yaml.h>

#include <easy/profiler.h>

#include <libsbx/units/time.hpp>

#include <libsbx/utility/timer.hpp>
#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/target.hpp>

#include <libsbx/math/angle.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/devices/devices_module.hpp>
#include <libsbx/devices/window.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/point_light.hpp>
#include <libsbx/scenes/components/global_transform.hpp>
#include <libsbx/scenes/components/hierarchy.hpp>

namespace sbx::scenes {

scene::scene(const std::filesystem::path& path)
: _registry{}, 
  _root{_registry.create()},
  _camera{_registry.create()},
  _light{math::vector3{-1.0, -1.0, -1.0}, math::color::white()},
  _octree{math::volume{math::vector3{-1000.0f, -1000.0f, -1000.0f}, math::vector3{1000.0f, 1000.0f, 1000.0f}}} {
  // [NOTE] KAJ 2023-10-17 : Initialize root node
  const auto& root_id = add_component<scenes::id>(_root);
  _nodes.insert({root_id, _root});

  add_component<scenes::relationship>(_root, math::uuid::null());
  add_component<math::transform>(_root);
  add_component<scenes::tag>(_root, "ROOT");
  add_component<scenes::hierarchy>(_root);
  add_component<scenes::global_transform>(_root);

  // [NOTE] KAJ 2023-10-17 : Initialize camera node
  const auto& camera_id = add_component<scenes::id>(_camera);

  _nodes.insert({camera_id, _camera});

  add_component<scenes::relationship>(_camera, root_id);
  get_component<scenes::relationship>(_root).add_child(camera_id);

  add_component<scenes::hierarchy>(_camera, _root);
  get_component<scenes::hierarchy>(_root).first_child = _camera;
  add_component<scenes::global_transform>(_camera);

  add_component<math::transform>(_camera);
  add_component<scenes::tag>(_camera, "Camera");

  auto& devices_module = core::engine::get_module<devices::devices_module>();
  auto& window = devices_module.window();

  add_component<scenes::camera>(_camera, math::angle{math::degree{50.0f}}, window.aspect_ratio(), 0.1f, 2000.0f);

  window.on_framebuffer_resized() += [this](const devices::framebuffer_resized_event& event) {
    auto& camera = get_component<scenes::camera>(_camera);
    camera.set_aspect_ratio(static_cast<std::float_t>(event.width) / static_cast<std::float_t>(event.height));
  };

  const auto scene = YAML::LoadFile(path.string());

  const auto& name = scene["name"].as<std::string>();

  const auto& metadata = scene["metadata"];

  _load_assets(scene["assets"]);
  _load_nodes(scene["nodes"]);
}

auto scene::create_child_node(const node_type parent, const std::string& tag, const math::transform& transform) -> node_type {
  auto node = _registry.create();

  const auto& id = add_component<scenes::id>(node);

  _nodes.insert({id, node});

  add_component<scenes::relationship>(node, get_component<scenes::id>(parent));
  get_component<scenes::relationship>(parent).add_child(id);

  auto& hierarchy = add_component<scenes::hierarchy>(node, parent);

  auto& parent_hierarchy = get_component<scenes::hierarchy>(parent);

  if (parent_hierarchy.first_child != node::null) {
    auto& first_child_hierarchy = get_component<scenes::hierarchy>(parent_hierarchy.first_child);
    first_child_hierarchy.previous_sibling = node;
    hierarchy.next_sibling = parent_hierarchy.first_child;
  } 

  parent_hierarchy.first_child = node;

  add_component<scenes::global_transform>(node);

  add_component<math::transform>(node, transform);

  add_component<scenes::tag>(node, !tag.empty() ? tag : scenes::tag{"Node"});

  return node;
}

auto scene::create_node(const std::string& tag, const math::transform& transform) -> node_type {
  return create_child_node(_root, tag, transform);
}

auto scene::destroy_node(const node_type node) -> void {
  const auto& id = get_component<scenes::id>(node);
  const auto& relationship = get_component<scenes::relationship>(node);

  for (auto& child_id : relationship.children()) {
    if (auto child = find_node(child_id); child != node::null) {
      destroy_node(child);
    }
  }

  if (auto entry = _nodes.find(id); entry != _nodes.end()) {
    get_component<scenes::relationship>(entry->second).remove_child(id);
  } else {
    utility::logger<"scenes">::warn("Node '{}' has invalid parent", get_component<scenes::tag>(node));
  }

  _nodes.erase(id);

  _registry.destroy(node);
}

auto scene::world_transform(const node_type node) -> math::matrix4x4 {
  EASY_FUNCTION();

  // [TODO] KAJ 2025-05-03 : FIX THIS! THE PERFORMANCE IS TERRIBLE!

  utility::assert_that(has_component<scenes::global_transform>(node), "Node has no global_transform component");

  const auto& transform = get_component<math::transform>(node);
  const auto& global_transform = get_component<scenes::global_transform>(node);

  if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
    if (transform.is_dirty()) {
      utility::logger<"scenes">::warn("Node '{}' has dirty transform", get_component<scenes::tag>(node));
    }
  }

  // const auto parent = _nodes.at(relationship.parent());

  // auto world = math::matrix4x4::identity;

  // if (get_component<scenes::id>(parent) != get_component<scenes::id>(_root)) {
  //   world = world_transform(parent);
  // }

  return global_transform.model;
}

auto scene::world_normal(const node_type node) -> math::matrix4x4 {
  EASY_FUNCTION();

  utility::assert_that(has_component<scenes::global_transform>(node), "Node has no global_transform component");

  const auto& global_transform = get_component<scenes::global_transform>(node);

  return global_transform.normal;
}

auto scene::world_position(const node_type node) -> math::vector3 {
  EASY_FUNCTION();

  return math::vector3{world_transform(node)[3]};
}

auto scene::_load_assets(const YAML::Node& assets) -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  for (const auto& mesh : assets["meshes"]) {
    const auto& name = mesh["name"].as<std::string>();
    const auto& path = mesh["path"].as<std::string>();
    const auto& id = mesh["id"].as<std::string>();
  }

  for (const auto& mesh : assets["materials"]) {
    const auto& name = mesh["name"].as<std::string>();
    const auto& path = mesh["path"].as<std::string>();
    const auto& id = mesh["id"].as<std::string>();
  }
}

auto scene::_load_nodes(const YAML::Node& assets) -> void {

}

} // namespace sbx::scenes
