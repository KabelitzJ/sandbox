#ifndef DEMO_TERRAIN_TERRAIN_SUBRENDERER_HPP_
#define DEMO_TERRAIN_TERRAIN_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <demo/terrain/vertex.hpp>
#include <demo/terrain/pipeline.hpp>
#include <demo/terrain/mesh.hpp>
#include <demo/terrain/planet_generator_task.hpp>

namespace demo {

struct terrain {
  sbx::math::uuid mesh_id;
  sbx::math::color tint;
  sbx::math::uuid grass_albedo_id;
  sbx::math::uuid dirt_albedo_id;
}; // struct terrain

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

    auto terrain_nodes = scene.query<demo::terrain>();

    _pipeline.bind(command_buffer);

    // auto model = sbx::math::matrix4x4::identity;
    // auto normal = sbx::math::matrix4x4::transposed(sbx::math::matrix4x4::inverted(model));

    // _push_handler.push("model", model);
    // _push_handler.push("normal", normal);
    // _push_handler.push("tint", sbx::math::color::red);

    // _descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    // _descriptor_handler.push("data", _push_handler);

    // if (!_descriptor_handler.update(_pipeline)) {
    //   return;
    // }

    // _descriptor_handler.bind_descriptors(command_buffer);
    // _push_handler.bind(command_buffer, _pipeline);

    // command_buffer.bind_vertex_buffer(0u, _planet_generator_task->vertices());
    // command_buffer.bind_index_buffer(_planet_generator_task->indices(), 0u, VK_INDEX_TYPE_UINT32);

    // command_buffer.draw_indexed(3840u, 1u, 0u, 0u, 0u);

    for (auto& node : terrain_nodes) {
      auto& terrain = node.get_component<demo::terrain>();
      auto& mesh = graphics_module.get_asset<demo::mesh>(terrain.mesh_id);

      auto model = scene.world_transform(node);
      auto normal = sbx::math::matrix4x4::transposed(sbx::math::matrix4x4::inverted(model));

      _push_handler.push("model", model);
      _push_handler.push("normal", normal);
      _push_handler.push("tint", terrain.tint);

      _storage_handler.push(std::span<const std::float_t>{mesh.heights()});

      _descriptor_handler.push("uniform_scene", _scene_uniform_handler);
      _descriptor_handler.push("data", _push_handler);
      _descriptor_handler.push("buffer_heightmap", _storage_handler);
      _descriptor_handler.push("grass_albedo_image", graphics_module.get_asset<sbx::graphics::image2d>(terrain.grass_albedo_id));
      _descriptor_handler.push("dirt_albedo_image", graphics_module.get_asset<sbx::graphics::image2d>(terrain.dirt_albedo_id));

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
