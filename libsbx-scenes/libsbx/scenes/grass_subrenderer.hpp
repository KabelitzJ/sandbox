#ifndef LIBSBX_SCENES_GRASS_SUBRENDERER_HPP_
#define LIBSBX_SCENES_GRASS_SUBRENDERER_HPP_

#include <optional>

#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/buffers/push_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/scenes/scenes_module.hpp>

namespace sbx::scenes {

constexpr static auto num_blades = 1u << 13;
constexpr static auto min_height = 1.3f;
constexpr static auto max_height = 2.5f;
constexpr static auto min_width = 0.1f;
constexpr static auto max_width = 0.14f;
constexpr static auto min_bend = 7.0f;
constexpr static auto max_bend = 13.0f;

struct grass_blade {
  math::vector4 v0;
  math::vector4 v1;
  math::vector4 v2;
  math::vector4 up;
}; // struct grass_blade

constexpr auto operator==(const grass_blade& lhs, const grass_blade& rhs) noexcept -> bool {
  return lhs.v0 == rhs.v0 && lhs.v1 == rhs.v1 && lhs.v2 == rhs.v2 && lhs.up == rhs.up;
}

struct blade_draw_indirect {
  std::uint32_t vertex_count;
  std::uint32_t instance_count;
  std::uint32_t first_vertex;
  std::uint32_t first_instance;
}; // struct blade_draw_indirect

} // namespace sbx::scenes

template<>
struct sbx::graphics::vertex_input<sbx::scenes::grass_blade> {
  static auto description() -> sbx::graphics::vertex_input_description {
    auto result = vertex_input_description{};

    result.binding_descriptions.push_back(VkVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(sbx::scenes::grass_blade),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = offsetof(sbx::scenes::grass_blade, v0)
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = offsetof(sbx::scenes::grass_blade, v1)
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 2,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = offsetof(sbx::scenes::grass_blade, v2)
    });

    result.attribute_descriptions.push_back(VkVertexInputAttributeDescription{
      .location = 3,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = offsetof(sbx::scenes::grass_blade, up)
    });

    return result;
  }
}; // struct sbx::graphics::vertex_input

namespace sbx::scenes {

class grass_subrenderer final : public sbx::graphics::subrenderer {

  class pipeline : public sbx::graphics::graphics_pipeline {

    inline static const auto pipeline_definition = sbx::graphics::pipeline_definition{
      .depth = sbx::graphics::depth::read_only,
      .uses_transparency = true,
      .rasterization_state = sbx::graphics::rasterization_state{
        .polygon_mode = sbx::graphics::polygon_mode::fill,
        .cull_mode = sbx::graphics::cull_mode::none,
        .front_face = sbx::graphics::front_face::counter_clockwise
      },
      .primitive_topology = sbx::graphics::primitive_topology::line_list,
      .vertex_input = sbx::graphics::vertex_input<sbx::scenes::grass_blade>::description()
    };
  
    using base_type = sbx::graphics::graphics_pipeline;
  
  public:
  
    pipeline(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
    : base_type{path, stage, pipeline_definition} { }
  
    ~pipeline() override = default;
  
  }; // class pipeline

public:

  grass_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : sbx::graphics::subrenderer{stage},
    _pipeline{path, stage},
    _push_handler{_pipeline},
    _storage_buffer{std::make_optional<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)},
    _descriptor_handler{_pipeline, 0u} { }

  ~grass_subrenderer() override {

  }

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& camera = scene.get_component<sbx::scenes::camera>(camera_node);

    const auto& projection = camera.projection();

    const auto& camera_global_transform = scene.get_component<sbx::scenes::global_transform>(camera_node);

    const auto view = sbx::math::matrix4x4::inverted(camera_global_transform.model);
    
    _pipeline.bind(command_buffer);

    _push_handler.push("mvp", projection * view);
    _push_handler.push("vertex_data", _storage_buffer->address());

    // _descriptor_handler.push("buffer_vertex_data", _storage_handler);
    // _descriptor_handler.push("push", _push_handler);

    if (!_descriptor_handler.update(_pipeline)) {
      return;
    }

    _descriptor_handler.bind_descriptors(command_buffer);
    _push_handler.bind(command_buffer);
  }

private:

  pipeline _pipeline;

  sbx::graphics::push_handler _push_handler;

  std::optional<graphics::storage_buffer> _storage_buffer;

  sbx::graphics::descriptor_handler _descriptor_handler;

}; // class debug_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_GRASS_SUBRENDERER_HPP_
