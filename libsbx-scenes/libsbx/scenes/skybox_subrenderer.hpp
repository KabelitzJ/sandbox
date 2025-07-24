#ifndef LIBSBX_SCENES_SKYBOX_SUBRENDERER_HPP_
#define LIBSBX_SCENES_SKYBOX_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/graphics/images/cube_image.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/pipeline/vertex_input_description.hpp>
#include <libsbx/graphics/pipeline/mesh.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/buffers/uniform_handler.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/components/skybox.hpp>
#include <libsbx/scenes/components/camera.hpp>

namespace sbx::scenes {

struct vertex3d {
  math::vector3 position;
}; // struct vertex3d

class mesh : public graphics::mesh<vertex3d> {

public:

  mesh(std::vector<vertex3d>&& vertices, std::vector<std::uint32_t>&& indices)
  : graphics::mesh<vertex3d>{std::move(vertices), std::move(indices)} { }

  ~mesh() override = default;
  
}; // class mesh

class skybox_subrenderer : public sbx::graphics::subrenderer {

  class pipeline : public graphics::graphics_pipeline {

    inline static const auto pipeline_definition = graphics::pipeline_definition{
      .depth = graphics::depth::read_write,
      .uses_transparency = false,
      .rasterization_state = graphics::rasterization_state{
        .polygon_mode = graphics::polygon_mode::fill,
        .cull_mode = graphics::cull_mode::none,
        .front_face = graphics::front_face::counter_clockwise
      }
    };
  
    using base_type = graphics::graphics_pipeline;
  
  public:
  
    pipeline(const std::filesystem::path& path, const graphics::render_graph::graphics_pass& pass)
    : base_type{path, pass, pipeline_definition} { }
  
    ~pipeline() override = default;
  
  }; // class pipeline

public:

  skybox_subrenderer(const std::filesystem::path& path, const graphics::render_graph::graphics_pass& pass)
  : graphics::subrenderer{pass},
    _pipeline{path, pass},
    _descriptor_handler{_pipeline, 0u},
    _push_handler{_pipeline} {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto vertices = std::vector<vertex3d>{
      vertex3d{math::vector3{-10.0f,  10.0f, -10.0f}}, // 0
      vertex3d{math::vector3{-10.0f, -10.0f, -10.0f}}, // 1
      vertex3d{math::vector3{ 10.0f, -10.0f, -10.0f}}, // 2
      vertex3d{math::vector3{ 10.0f,  10.0f, -10.0f}}, // 3
      vertex3d{math::vector3{-10.0f,  10.0f,  10.0f}}, // 4
      vertex3d{math::vector3{-10.0f, -10.0f,  10.0f}}, // 5
      vertex3d{math::vector3{ 10.0f, -10.0f,  10.0f}}, // 6
      vertex3d{math::vector3{ 10.0f,  10.0f,  10.0f}}  // 7
    };

    auto indices = std::vector<std::uint32_t>{
      // Back face
      0, 1, 2,
      2, 3, 0,
      // Left face
      4, 5, 1,
      1, 0, 4,
      // Right face
      3, 2, 6,
      6, 7, 3,
      // Front face
      7, 6, 5,
      5, 4, 7,
      // Top face
      4, 0, 3,
      3, 7, 4,
      // Bottom face
      1, 5, 6,
      6, 2, 1
    };

    _skybox_id = assets_module.add_asset<scenes::mesh>(std::move(vertices), std::move(indices));
  }

  ~skybox_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& assets_module = core::engine::get_module<assets::assets_module>();
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto camera_node = scene.camera();
    
    if (!scene.has_component<scenes::skybox>(camera_node)) {
      utility::logger<"scenes">::warn("Skybox subrenderer: No camera node with skybox component found");
      return;
    }

    const auto& skybox = scene.get_component<scenes::skybox>(camera_node);

    const auto& camera = scene.get_component<scenes::camera>(camera_node);
    const auto& camera_transform = scene.get_component<math::transform>(camera_node);

    const auto& projection = camera.projection();
    _scene_uniform_handler.push("projection", projection);

    const auto view = math::matrix4x4::inverted(camera_transform.as_matrix());
    _scene_uniform_handler.push("view", view);

    const auto model = scene.world_transform(camera_node);
    _scene_uniform_handler.push("model", model);
    _scene_uniform_handler.push("tint", skybox.tint);

    auto& mesh = assets_module.get_asset<scenes::mesh>(_skybox_id);

    _pipeline.bind(command_buffer);

    _descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    _descriptor_handler.push("skybox", graphics_module.get_resource<graphics::cube_image>(skybox.cube_image));

    _push_handler.push("vertex_buffer", mesh.address());

    if (!_descriptor_handler.update(_pipeline)) {
      return;
    }

    _descriptor_handler.bind_descriptors(command_buffer);
    _push_handler.bind(command_buffer);

    mesh.bind(command_buffer);
    mesh.render(command_buffer, 1u);
  }

private:

  pipeline _pipeline;
  math::uuid _skybox_id;

  graphics::descriptor_handler _descriptor_handler;

  graphics::uniform_handler _scene_uniform_handler;
  graphics::push_handler _push_handler;

}; // class skybox_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SKYBOX_SUBRENDERER_HPP_
