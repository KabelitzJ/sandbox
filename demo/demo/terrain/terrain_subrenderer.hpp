#ifndef DEMO_TERRAIN_TERRAIN_SUBRENDERER_HPP_
#define DEMO_TERRAIN_TERRAIN_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <demo/terrain/vertex.hpp>
#include <demo/terrain/pipeline.hpp>
#include <demo/terrain/mesh.hpp>
#include <demo/terrain/planet_generator_task.hpp>
#include <demo/terrain/chunk.hpp>

namespace demo {

class terrain_subrenderer final : public sbx::graphics::subrenderer {

  using base = sbx::graphics::subrenderer;

public:

  terrain_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : base{stage},
    _pipeline{path, stage} { }

  ~terrain_subrenderer() override = default;

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& camera = camera_node.get_component<sbx::scenes::camera>();

    _scene_uniform_handler.push("projection", camera.projection());

    const auto& camera_transform = camera_node.get_component<sbx::math::transform>();

    _scene_uniform_handler.push("view", sbx::math::matrix4x4::inverted(camera_transform.as_matrix()));

    auto chunk_nodes = scene.query<demo::chunk>();

    _pipeline.bind(command_buffer);

    for (const auto& node : chunk_nodes) {
      const auto& chunk = node.get_component<demo::chunk>();
      const auto& mesh = graphics_module.get_asset<sbx::models::mesh>(chunk.mesh_id);

      const auto model = scene.world_transform(node);
      const auto normal = sbx::math::matrix4x4::transposed(sbx::math::matrix4x4::inverted(model));

      _push_handler.push("model", model);
      _push_handler.push("normal", normal);
      _push_handler.push("water_color", chunk.water_color);
      _push_handler.push("land_color", chunk.land_color);
      _push_handler.push("mountain_color", chunk.mountain_color);

      _descriptor_handler.push("uniform_scene", _scene_uniform_handler);
      _descriptor_handler.push("push", _push_handler);

      if (!_descriptor_handler.update(_pipeline)) {
        return;
      }

      _descriptor_handler.bind_descriptors(command_buffer);
      _push_handler.bind(command_buffer, _pipeline);

      mesh.render(command_buffer);
    }

  }

private:

  pipeline _pipeline;

  sbx::graphics::uniform_handler _scene_uniform_handler;
  sbx::graphics::push_handler _push_handler;
  sbx::graphics::descriptor_handler _descriptor_handler;
  sbx::graphics::storage_handler _storage_handler;

}; // class terrain_subrenderer

} // namespace demo::terrain

#endif // DEMO_TERRAIN_TERRAIN_SUBRENDERER_HPP_
