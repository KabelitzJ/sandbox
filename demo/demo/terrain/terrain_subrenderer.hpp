#ifndef DEMO_TERRAIN_TERRAIN_SUBRENDERER_HPP_
#define DEMO_TERRAIN_TERRAIN_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace demo {

class pipeline : public sbx::graphics::graphics_pipeline<sbx::models::vertex3d> {

  inline static constexpr auto pipeline_definition = sbx::graphics::pipeline_definition{
    .uses_depth = true,
    .uses_transparency = false,
    .rasterization_state = sbx::graphics::rasterization_state{
      .polygon_mode = sbx::graphics::polygon_mode::fill,
      .cull_mode = sbx::graphics::cull_mode::back,
      .front_face = sbx::graphics::front_face::counter_clockwise
    }
  };

  using base = sbx::graphics::graphics_pipeline<sbx::models::vertex3d>;

public:

  using vertex_type = sbx::models::vertex3d;

  pipeline(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : base{path, stage, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

struct terrain {
  sbx::math::uuid mesh_id;
  sbx::math::color tint;
  std::optional<sbx::math::uuid> texture_id;
  std::optional<sbx::math::uuid> normal_id;
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

    _images.clear();

    auto terrain_nodes = scene.query<demo::terrain>();

    _pipeline.bind(command_buffer);

    for (auto& node : terrain_nodes) {
      auto& terrain = node.get_component<demo::terrain>();
      auto& mesh = graphics_module.get_asset<sbx::models::mesh>(terrain.mesh_id);

      auto model = scene.world_transform(node);
      auto normal = sbx::math::matrix4x4::transposed(sbx::math::matrix4x4::inverted(model));

      const auto albedo_image_index = terrain.texture_id ? _images.push_back(*terrain.texture_id) : sbx::graphics::separate_image2d_array::max_size;
      const auto normal_image_index = terrain.normal_id ? _images.push_back(*terrain.normal_id) : sbx::graphics::separate_image2d_array::max_size;

      _push_handler.push("model", model);
      _push_handler.push("normal", normal);
      _push_handler.push("tint", terrain.tint);
      _push_handler.push("image_indices", sbx::math::vector4{albedo_image_index, normal_image_index, 0u, 0u});

      _descriptor_handler.push("uniform_scene", _scene_uniform_handler);
      _descriptor_handler.push("data", _push_handler);
      _descriptor_handler.push("images", _images);
      _descriptor_handler.push("images_sampler", _images_sampler);

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

  sbx::graphics::separate_sampler _images_sampler;
  sbx::graphics::separate_image2d_array _images;

}; // class terrain_subrenderer

} // namespace demo::terrain

#endif // DEMO_TERRAIN_TERRAIN_SUBRENDERER_HPP_
