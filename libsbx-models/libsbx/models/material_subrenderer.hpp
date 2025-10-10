#ifndef LIBSBX_MODELS_MATERIAL_SUBRENDERER_HPP_
#define LIBSBX_MODELS_MATERIAL_SUBRENDERER_HPP_

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

enum class material_feature : std::uint8_t {
  emission    = utility::bit_v<0>,
  normal_map  = utility::bit_v<1>, 
  occlusion   = utility::bit_v<2>, 
  height      = utility::bit_v<3>, 
  clearcoat   = utility::bit_v<4>, 
  anisotropy  = utility::bit_v<5>
}; // struct material_feature

struct material_key {

  std::uint64_t blend        : 2;
  std::uint64_t cull         : 2;
  std::uint64_t depth        : 2;
  std::uint64_t alpha        : 2;
  std::uint64_t double_sided : 1;
  std::uint64_t alpha_to_cov : 1;
  std::uint64_t _pad0        : 2;
  std::uint64_t feature_mask : 16;
  std::uint64_t _pad1        : 36;

  static auto hash(const material_key& key) -> std::uint64_t {
    return utility::djb2_hash{}({reinterpret_cast<const std::uint8_t*>(&key), sizeof(material_key)});
  }

  auto operator==(const material_key& other) const -> bool { 
    return std::memcmp(this, &other, sizeof(material_key)) == 0; 
  }

}; // struct material_key

static_assert(sizeof(material_key) == sizeof(std::uint64_t));
static_assert(alignof(material_key) >= alignof(std::uint64_t));

class material_subrenderer final : public graphics::subrenderer {

  inline static const auto pipeline_definition = graphics::pipeline_definition{
    .depth = graphics::depth::read_write,
    .uses_transparency = false,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::back,
      .front_face = graphics::front_face::counter_clockwise
    },
  };

  static auto _create_pipeline(const std::filesystem::path& path, const graphics::render_graph::graphics_pass& pass) -> graphics::graphics_pipeline_handle {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    return graphics_module.add_resource<graphics::graphics_pipeline>(path, pass, pipeline_definition);
  }

public:

  material_subrenderer(const graphics::render_graph::graphics_pass& pass)
  : graphics::subrenderer{pass} { }

  ~material_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    EASY_FUNCTION();

    SBX_SCOPED_TIMER("static_mesh_subrenderer");

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();
  }

private:

  struct pipeline_data {
    graphics::graphics_pipeline_handle pipeline;
    graphics::push_handler push_handler;
    graphics::descriptor_handler scene_descriptor_handler;
    std::uint32_t users;

    pipeline_data(const graphics::graphics_pipeline_handle& handle)
    : pipeline{handle},
      push_handler{pipeline},
      scene_descriptor_handler{pipeline, 0u},
      users{1u} { }

  }; // struct pipeline_data

  std::unordered_map<std::uint64_t, pipeline_data> _pipeline_cache;

}; // class material_subrenderer

} // namespace sbx::models

#endif // LIBSBX_MODELS_MATERIAL_SUBRENDERER_HPP_
