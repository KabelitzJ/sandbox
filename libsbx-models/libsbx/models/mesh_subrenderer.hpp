#ifndef LIBSBX_MODELS_MESH_SUBRENDERER_HPP_
#define LIBSBX_MODELS_MESH_SUBRENDERER_HPP_

#include <filesystem>
#include <unordered_set>
#include <ranges>
#include <algorithm>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>
#include <libsbx/graphics/buffers/uniform_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/script.hpp>
#include <libsbx/scenes/components/point_light.hpp>

#include <libsbx/models/vertex3d.hpp>
#include <libsbx/models/pipeline.hpp>

namespace sbx::models {

struct point_light {
  math::color color;
  math::vector3 position;
  std::float_t intensity;
}; // struct point_light

class mesh_subrenderer final : public graphics::subrenderer {

  inline static constexpr auto max_point_lights = std::size_t{16};

public:

  mesh_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _pipeline{path, stage},
    _camera_position{2.0f, 2.0f, 1.0f},
    _light_position{-1.0f, 3.0f, 1.0f} { }

  ~mesh_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    // [NOTE] KAJ 2023-10-25 : We need those when we want to load the shadow maps
    // auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    // auto& render_stage = graphics_module.render_stage(stage());

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& camera = camera_node.get_component<scenes::camera>();

    if (!camera.is_active()) {
      core::logger::warn("Scene does not have an active camera");
      return;
    }

    _scene_uniform_handler.push("projection", camera.projection());

    auto& camera_transform = camera_node.get_component<math::transform>();

    _scene_uniform_handler.push("view", math::matrix4x4::inverted(camera_transform.as_matrix()));

    _scene_uniform_handler.push("camera_position", camera_transform.position()); 

    auto light_nodes = scene.query<scenes::point_light>();

    auto lights = std::vector<models::point_light>{};
    auto point_light_count = std::uint32_t{0};

    for (const auto& node : light_nodes) {
      const auto& light = node.get_component<scenes::point_light>();
      const auto& transform = node.get_component<math::transform>();

      lights.push_back(models::point_light{light.color(), transform.position(), light.radius()});
      
      ++point_light_count;

      if (point_light_count >= max_point_lights) {
        break;
      }
    }

    _lights_storage_handler.push(std::span<const models::point_light>{lights.data(), point_light_count});
    _scene_uniform_handler.push("point_light_count", point_light_count);

    auto directional_light_nodes = scene.query<scenes::directional_light>();

    if (directional_light_nodes.empty()) {
      core::logger::warn("Scene does not have a directional light");
    }

    if (directional_light_nodes.size() > 1) {
      core::logger::warn("Scene has more than one directional light. Discarding all but the first one");
    }

    auto& directional_light_node = directional_light_nodes[0];
    auto& directional_light_transform = directional_light_node.get_component<math::transform>();

    const auto view = math::matrix4x4::look_at(directional_light_transform.position(), math::vector3::zero, math::vector3::up);
    const auto projection = math::matrix4x4::orthographic(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);

    _scene_uniform_handler.push("light_space", math::matrix4x4{projection * view});

    for (auto entry = _uniform_data.begin(); entry != _uniform_data.end();) {
      if (_used_uniforms.contains(entry->first)) {
        ++entry;
      } else {
        entry = _uniform_data.erase(entry);
      }
    }

    _used_uniforms.clear();

    auto mesh_nodes = scene.query<scenes::static_mesh>();

    for (auto& node : mesh_nodes) {
      _used_uniforms.insert(node.get_component<scenes::id>());
      _render_node(node, command_buffer);
    }
  }

private:

  auto _render_node(scenes::node& node, graphics::command_buffer& command_buffer) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    // [NOTE] KAJ 2023-10-25 : We need this for loading the shadow map in the future
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    const auto& static_mesh = node.get_component<scenes::static_mesh>();
    const auto& id = node.get_component<scenes::id>();

    auto& mesh = assets_module.get_asset<models::mesh>(static_mesh.mesh_id());
    auto& image = assets_module.get_asset<graphics::image2d>(static_mesh.texture_id());
    const auto& tint = static_mesh.tint();

    _pipeline.bind(command_buffer);

    auto world_transform = scene.world_transform(node);

    // [NOTE] KAJ 2023-10-26 : We want to insert a new object into the map when it does not exist
    auto& uniform_data = _uniform_data[id];
    
    auto& push_handler = uniform_data.push_handler;
    auto& descriptor_handler = uniform_data.descriptor_handler;

    push_handler.push("model", world_transform);
    push_handler.push("normal", math::matrix4x4::transposed(math::matrix4x4::inverted(world_transform)));
    push_handler.push("tint", tint);
    // [NOTE] KAJ 2023-10-29 : Meshes on light sources should not be lit
    push_handler.push("uses_lighting", node.has_component<scenes::point_light>() ? 0 : 1);

    descriptor_handler.push("object", push_handler);
    descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    descriptor_handler.push("buffer_point_lights", _lights_storage_handler);
    descriptor_handler.push("image", image);
    descriptor_handler.push("shadow_map", graphics_module.attachment("shadow_map"));

    if (!descriptor_handler.update(_pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);
    push_handler.bind(command_buffer, _pipeline);

    mesh.render(command_buffer);
  }

  struct uniform_data {
    graphics::push_handler push_handler;
    graphics::descriptor_handler descriptor_handler;
  }; // struct uniform_data

  pipeline _pipeline;

  math::vector3 _camera_position;
  math::vector3 _light_position;

  std::unordered_map<math::uuid, uniform_data> _uniform_data;
  std::unordered_set<math::uuid> _used_uniforms;

  graphics::uniform_handler _scene_uniform_handler;
  graphics::storage_handler _lights_storage_handler;

}; // class mesh_subrenderer

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_SUBRENDERER_HPP_
