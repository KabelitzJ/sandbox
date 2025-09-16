#ifndef LIBSBX_MODELS_STATIC_MESH_SUBRENDERER_HPP_
#define LIBSBX_MODELS_STATIC_MESH_SUBRENDERER_HPP_

#include <cstddef>
#include <filesystem>
#include <unordered_set>
#include <ranges>
#include <algorithm>

#include <easy/profiler.h>

#include <fmt/format.h>

#include <tsl/robin_map.h>
#include <tsl/robin_set.h>

#include <range/v3/view/enumerate.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/containers/octree.hpp>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/timer.hpp>
#include <libsbx/utility/layout.hpp>
#include <libsbx/utility/iterator.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/draw_list.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>
#include <libsbx/graphics/buffers/uniform_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>
#include <libsbx/graphics/images/image2d.hpp>
#include <libsbx/graphics/images/separate_image2d_array.hpp>
#include <libsbx/graphics/images/separate_sampler.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>

#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/point_light.hpp>
#include <libsbx/scenes/components/global_transform.hpp>
#include <libsbx/scenes/components/selection_tag.hpp>

#include <libsbx/models/vertex3d.hpp>
#include <libsbx/models/mesh.hpp>
#include <libsbx/models/static_mesh_draw_list.hpp>

namespace sbx::models {

namespace detail {

template<scenes::material_type Type>
struct static_mesh_subrenderer_traits;

template<>
struct static_mesh_subrenderer_traits<scenes::material_type::opaque> {
  inline static constexpr auto depth = graphics::depth::read_write;
  inline static constexpr auto uses_transparency = false;
  inline static constexpr auto cull_mode = graphics::cull_mode::back;
  inline static const auto scope = utility::hashed_string{"opaque"};
  inline static const auto instance_data_buffer_name = static_mesh_draw_list::opaque_instance_data_buffer_name;
  inline static const auto draw_commands_buffer_name = static_mesh_draw_list::opaque_draw_commands_buffer_name;
}; // struct static_mesh_subrenderer_traits

template<>
struct static_mesh_subrenderer_traits<scenes::material_type::masked> {
  inline static constexpr auto depth = graphics::depth::read_write;
  inline static constexpr auto uses_transparency = false;
  inline static constexpr auto cull_mode = graphics::cull_mode::none;
  inline static const auto scope = utility::hashed_string{"masked"};
  inline static const auto instance_data_buffer_name = static_mesh_draw_list::masked_instance_data_buffer_name;
  inline static const auto draw_commands_buffer_name = static_mesh_draw_list::masked_draw_commands_buffer_name;
}; // struct static_mesh_subrenderer_traits

template<>
struct static_mesh_subrenderer_traits<scenes::material_type::transparent> {
  inline static constexpr auto depth = graphics::depth::read_only;
  inline static constexpr auto uses_transparency = true;
  inline static constexpr auto cull_mode = graphics::cull_mode::none;
  inline static const auto scope = utility::hashed_string{"transparent"};
  inline static const auto instance_data_buffer_name = static_mesh_draw_list::transparent_instance_data_buffer_name;
  inline static const auto draw_commands_buffer_name = static_mesh_draw_list::transparent_draw_commands_buffer_name;
}; // struct static_mesh_subrenderer_traits

} // namespace detail

template<scenes::material_type Type>
class static_mesh_subrenderer final : public graphics::subrenderer {

  using traits = detail::static_mesh_subrenderer_traits<Type>;

  inline static const auto pipeline_definition = graphics::pipeline_definition{
    .depth = traits::depth,
    .uses_transparency = traits::uses_transparency,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = traits::cull_mode,
      .front_face = graphics::front_face::counter_clockwise
    },
    // .vertex_input = graphics::vertex_input<models::vertex3d>::description()
  };

  // class pipeline : public graphics::graphics_pipeline {


  //   using base_type = graphics::graphics_pipeline;

  // public:

  //   pipeline(const std::filesystem::path& path, const graphics::render_graph::graphics_pass& pass)
  //   : base_type{path, pass, pipeline_definition} { }

  //   ~pipeline() override = default;

  // }; // class pipeline

  static auto _create_pipeline(const std::filesystem::path& path, const graphics::render_graph::graphics_pass& pass) -> graphics::graphics_pipeline_handle {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    return graphics_module.add_resource<graphics::graphics_pipeline>(path, pass, pipeline_definition);
  }

public:

  static_mesh_subrenderer(const std::filesystem::path& path, const graphics::render_graph::graphics_pass& pass)
  : graphics::subrenderer{pass},
    _pipeline{_create_pipeline(path, pass)},
    _push_handler{_pipeline},
    _scene_descriptor_handler{_pipeline, 0u} {
    // auto& assets_module = core::engine::get_module<assets::assets_module>();

    // assets_module.register_asset<models::mesh>(
    //   "models::mesh",
    //   [](const models::mesh& mesh) -> void {

    //   },
    //   []() -> void {

    //   }
    // );
  }

  ~static_mesh_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    EASY_FUNCTION();

    SBX_SCOPED_TIMER("static_mesh_subrenderer");

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    EASY_BLOCK("gathering scene data");

    auto camera_node = scene.camera();

    auto& camera = scene.get_component<scenes::camera>(camera_node);

    const auto& projection = camera.projection();

    _scene_uniform_handler.push("projection", projection);

    const auto& camera_transform = scene.get_component<scenes::transform>(camera_node);
    const auto& camera_global_transform = scene.get_component<scenes::global_transform>(camera_node);

    const auto view = math::matrix4x4::inverted(scene.world_transform(camera_node));

    _scene_uniform_handler.push("view", view);

    _scene_uniform_handler.push("camera_position", camera_transform.position());

    const auto& scene_light = scene.light();

    _scene_uniform_handler.push("light_space", scene.light_space());

    _scene_uniform_handler.push("light_direction", sbx::math::vector3::normalized(scene_light.direction()));
    _scene_uniform_handler.push("light_color", scene_light.color());

    _scene_uniform_handler.push("time", core::engine::time().value());

    EASY_END_BLOCK;

    auto& draw_list = pass().draw_list("static_mesh");

    auto& pipeline = graphics_module.get_resource<graphics::graphics_pipeline>(_pipeline);

    pipeline.bind(command_buffer);

    _scene_descriptor_handler.push("scene", _scene_uniform_handler);
    _scene_descriptor_handler.push("images_sampler", draw_list->sampler());
    _scene_descriptor_handler.push("images", draw_list->images());

    if (!_scene_descriptor_handler.update(pipeline)) {
      return;
    }

    _scene_descriptor_handler.bind_descriptors(command_buffer);

    _push_handler.push("transform_data_buffer", draw_list->buffer(static_mesh_draw_list::transform_data_buffer_name).address());
    _push_handler.push("instance_data_buffer", draw_list->buffer(traits::instance_data_buffer_name).address());

    for (const auto& [mesh_id, range] : draw_list->draw_ranges(traits::scope)) {
      auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);
      
      mesh.bind(command_buffer);
      
      _push_handler.push("vertex_buffer", mesh.address());

      _push_handler.bind(command_buffer);

      command_buffer.draw_indexed_indirect(draw_list->buffer(traits::draw_commands_buffer_name), range.offset, range.count);
    }
  }

private:

  graphics::graphics_pipeline_handle _pipeline;

  graphics::push_handler _push_handler;
  graphics::descriptor_handler _scene_descriptor_handler;
  graphics::uniform_handler _scene_uniform_handler;

}; // class mesh_subrenderer

using opaque_static_mesh_subrenderer = static_mesh_subrenderer<scenes::material_type::opaque>;

using masked_static_mesh_subrenderer = static_mesh_subrenderer<scenes::material_type::masked>;

using transparent_static_mesh_subrenderer = static_mesh_subrenderer<scenes::material_type::transparent>;

} // namespace sbx::models

#endif // LIBSBX_MODELS_STATIC_MESH_SUBRENDERER_HPP_
