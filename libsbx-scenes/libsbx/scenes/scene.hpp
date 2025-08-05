#ifndef LIBSBX_SCENES_SCENE_HPP_
#define LIBSBX_SCENES_SCENE_HPP_

#include <unordered_map>
#include <memory>
#include <utility>
#include <ranges>
#include <vector>
#include <typeindex>
#include <filesystem>
#include <numbers>
#include <algorithm>
#include <ranges>

#include <range/v3/all.hpp>

#include <yaml-cpp/yaml.h>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/utility/hashed_string.hpp>
#include <libsbx/utility/iterator.hpp>

#include <libsbx/containers/octree.hpp>

#include <libsbx/ecs/registry.hpp>
#include <libsbx/ecs/entity.hpp>

#include <libsbx/math/transform.hpp>
#include <libsbx/math/uuid.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/quaternion.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/images/image2d.hpp>
#include <libsbx/graphics/images/cube_image.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/components/directional_light.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/selection_tag.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/hierarchy.hpp>
#include <libsbx/scenes/components/camera.hpp>

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

  auto create_child_node(const node_type parent, const std::string& tag = "", const math::transform& transform = math::transform{}, const selection_tag& selection_tag = selection_tag::null) -> node_type;

  auto create_node(const std::string& tag = "", const math::transform& transform = math::transform{}, const selection_tag& selection_tag = selection_tag::null) -> node_type;  

  auto destroy_node(const node_type node) -> void;

  auto camera() -> node_type {
    return _camera;
  }

  auto set_active_camera(const node_type camera) -> void {
    _camera = camera;
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
  auto add_component(const node_type node, Args&&... args) -> decltype(auto) {
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
    const auto& camera = get_component<scenes::camera>(_camera);
    const auto& camera_transform = get_component<math::transform>(_camera);

    const auto camera_view = math::matrix4x4::inverted(camera_transform.as_matrix());
    const auto camera_projection = camera.projection(0.1f, 100.0f);

    const auto inverse_view_projection = math::matrix4x4::inverted(camera_projection * camera_view);

    // auto frustum_corners_world = std::array<math::vector3, 8>{};

    const auto frustum_corners_clip = std::array<math::vector4, 8>{
      math::vector4{-1, -1, -1, 1},
      math::vector4{ 1, -1, -1, 1},
      math::vector4{ 1,  1, -1, 1},
      math::vector4{-1,  1, -1, 1},
      math::vector4{-1, -1,  1, 1},
      math::vector4{ 1, -1,  1, 1},
      math::vector4{ 1,  1,  1, 1},
      math::vector4{-1,  1,  1, 1},
    };

    const auto to_world_transform = [&inverse_view_projection](const auto& corner) {
      const auto frustum_corner = inverse_view_projection * corner;

      return math::vector3{frustum_corner} / frustum_corner.w();
    };

    const auto frustum_corners_world = frustum_corners_clip | ranges::views::transform(to_world_transform) | ranges::to<std::vector>();

    for (int i = 0; i < 8; ++i) {
      utility::logger<"scenes">::debug("corner[{}]: {}", i, frustum_corners_world[i]);
    }

    const auto center = std::accumulate(frustum_corners_world.begin(), frustum_corners_world.end(), math::vector3::zero) / 8.0f;

    utility::logger<"scenes">::debug("center: {}", center);

    const auto light_position = center - _light.direction() * 20.0f; // Move back along light dir
    const auto light_view = math::matrix4x4::look_at(light_position, center, math::vector3::up);

    const auto min_max_transform = [&light_view](const auto& bounds, const auto& corner) {
      const auto corner_light_space = light_view * math::vector4(corner, 1.0f);

      return std::pair{
        math::vector3::min(bounds.first, math::vector3(corner_light_space)),
        math::vector3::max(bounds.second, math::vector3(corner_light_space))
      };
    };

    const auto [min, max] = std::accumulate(frustum_corners_world.begin(), frustum_corners_world.end(), std::pair{math::vector3{std::numeric_limits<std::float_t>::max()}, math::vector3{-std::numeric_limits<std::float_t>::max()}}, min_max_transform);

    utility::logger<"scenes">::debug("min: {} max: {}", min, max);

    auto light_projection = math::matrix4x4::orthographic(min.x(), max.x(), min.y(), max.y(), -max.z(), -min.z());
    // const auto light_projection = math::matrix4x4::orthographic(-40.0f, 40.0f, -40.0f, 40.0f, 0.1f, 300.0f);

    return light_projection * light_view;

    // const auto position = _light.direction() * -20.0f;

    // const auto light_view = math::matrix4x4::look_at(position, position + _light.direction() * 20.0f, math::vector3::up);
    // const auto light_projection = math::matrix4x4::orthographic(-40.0f, 40.0f, -40.0f, 40.0f, 0.1f, 300.0f);

    // return light_projection * light_view;
  }

  auto find_node(const scenes::id& id) -> node_type {
    if (auto entry = _nodes.find(id); entry != _nodes.end()) {
      return entry->second;
    } 
      
    return node_type::null;
  }

  auto save(const std::filesystem::path& path)-> void;

  template<typename... Args>
  auto add_image(const utility::hashed_string& name, Args&&... args) -> void {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    _image_ids.emplace(name, graphics_module.add_resource<graphics::image2d>(std::forward<Args>(args)...));
  }

  auto get_image(const utility::hashed_string& name) -> graphics::image2d_handle {
    return _image_ids.at(name);
  }

  template<typename... Args>
  auto add_cube_image(const utility::hashed_string& name, Args&&... args) -> void {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    _cube_image_ids.emplace(name, graphics_module.add_resource<graphics::cube_image>(std::forward<Args>(args)...));
  }

  auto get_cube_image(const utility::hashed_string& name) -> graphics::cube_image2d_handle {
    return _cube_image_ids.at(name);
  }

  template<typename Mesh, typename... Args>
  auto add_mesh(const utility::hashed_string& name, Args&&... args) -> void {
    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

    _mesh_ids.emplace(name, assets_module.add_asset<Mesh>(std::forward<Args>(args)...));
  }

  auto get_mesh(const utility::hashed_string& name) -> math::uuid {
    return _mesh_ids.at(name);
  }

  template<typename Material, typename... Args>
  auto add_material(const utility::hashed_string& name, Args&&... args) -> void {
    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

    _materials_ids.emplace(name, assets_module.add_asset<Material>(std::forward<Args>(args)...));
  }

  auto get_material(const utility::hashed_string& name) -> math::uuid {
    if (auto entry = _materials_ids.find(name); entry != _materials_ids.end()) {
      return entry->second;
    }

    throw utility::runtime_error{"Material '{}' is not loaded in scene '{}'", name.str(), _name};
  }

private:

  auto _save_assets(YAML::Emitter& emitter) -> void;

  auto _save_meshes(YAML::Emitter& emitter) -> void;

  auto _save_textures(YAML::Emitter& emitter) -> void;

  auto _save_node(YAML::Emitter& emitter, const node_type node) -> void;
  
  auto _save_components(YAML::Emitter& emitter, const node_type node) -> void;

  auto _load_assets(const YAML::Node& assets) -> void;

  auto _load_nodes(const YAML::Node& nodes) -> void;

  std::unordered_map<math::uuid, node_type> _nodes;

  registry_type _registry;
  node_type _root;
  node_type _camera;

  std::string _name;

  containers::octree<math::uuid> _octtree;

  directional_light _light;

  std::unordered_map<utility::hashed_string, graphics::image2d_handle> _image_ids;
  std::unordered_map<utility::hashed_string, graphics::cube_image2d_handle> _cube_image_ids;
  std::unordered_map<utility::hashed_string, math::uuid> _mesh_ids;
  std::unordered_map<utility::hashed_string, math::uuid> _materials_ids;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
