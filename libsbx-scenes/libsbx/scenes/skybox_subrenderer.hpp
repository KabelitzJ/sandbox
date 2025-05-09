#ifndef LIBSBX_SCENES_SKYBOX_SUBRENDERER_HPP_
#define LIBSBX_SCENES_SKYBOX_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/graphics/images/cube_image.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/pipeline/vertex_input_description.hpp>
#include <libsbx/graphics/pipeline/mesh.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/buffers/uniform_handler.hpp>

#include <libsbx/scenes/components/skybox.hpp>
#include <libsbx/scenes/components/camera.hpp>

namespace sbx::scenes {

struct vertex3d {
  math::vector3 position;
}; // struct vertex3d

} // namespace sbx::scenes

template<>
struct sbx::graphics::vertex_input<sbx::scenes::vertex3d> {
  static auto description() -> sbx::graphics::vertex_input_description {
    auto binding_descriptions = std::vector<VkVertexInputBindingDescription>{};

    binding_descriptions.push_back(VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(sbx::scenes::vertex3d),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    });

    auto attribute_descriptions = std::vector<VkVertexInputAttributeDescription>{};

    attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(sbx::scenes::vertex3d, position)
    });

    return sbx::graphics::vertex_input_description{std::move(binding_descriptions), std::move(attribute_descriptions)};
  }
}; // struct sbx::graphics::vertex_input<sbx::models::vertex3d>

namespace sbx::scenes {

class mesh : public graphics::mesh<vertex3d> {

public:

  mesh(std::vector<vertex3d>&& vertices, std::vector<std::uint32_t>&& indices)
  : graphics::mesh<vertex3d>{std::move(vertices), std::move(indices)} { }

  ~mesh() override = default;
  
}; // class mesh

class skybox_subrenderer : public sbx::graphics::subrenderer {

  class pipeline : public graphics::graphics_pipeline<vertex3d> {

    inline static constexpr auto pipeline_definition = graphics::pipeline_definition{
      .depth = graphics::depth::read_write,
      .uses_transparency = false,
      .rasterization_state = graphics::rasterization_state{
        .polygon_mode = graphics::polygon_mode::fill,
        .cull_mode = graphics::cull_mode::none,
        .front_face = graphics::front_face::counter_clockwise
      }
    };
  
    using base_type = graphics::graphics_pipeline<vertex3d>;
  
  public:
  
    using vertex_type = vertex3d;
  
    pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
    : base_type{path, stage, pipeline_definition} { }
  
    ~pipeline() override = default;
  
  }; // class pipeline

public:

  skybox_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _pipeline{path, stage} {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

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

    _skybox_id = graphics_module.add_asset<scenes::mesh>(std::move(vertices), std::move(indices));
  }

  ~skybox_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
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
    _object_uniform_handler.push("model", model);
    _object_uniform_handler.push("tint", skybox.tint);

    _pipeline.bind(command_buffer);

    _descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    _descriptor_handler.push("uniform_object", _object_uniform_handler);
    _descriptor_handler.push("skybox", graphics_module.get_asset<graphics::cube_image>(skybox.cube_image));

    if (!_descriptor_handler.update(_pipeline)) {
      return;
    }

    _descriptor_handler.bind_descriptors(command_buffer);

    auto& mesh = graphics_module.get_asset<scenes::mesh>(_skybox_id);

    mesh.render(command_buffer, 1u);
  }

private:

  pipeline _pipeline;
  math::uuid _skybox_id;

  graphics::descriptor_handler _descriptor_handler;

  graphics::uniform_handler _scene_uniform_handler;
  graphics::uniform_handler _object_uniform_handler;

}; // class skybox_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SKYBOX_SUBRENDERER_HPP_
