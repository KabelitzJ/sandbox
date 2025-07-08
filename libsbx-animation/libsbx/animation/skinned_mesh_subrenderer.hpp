#ifndef LIBSBX_ANIMATION_SKINNED_MESH_SUBRENDERER_HPP_
#define LIBSBX_ANIMATION_SKINNED_MESH_SUBRENDERER_HPP_

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

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>
#include <libsbx/graphics/buffers/uniform_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>
#include <libsbx/graphics/images/image2d.hpp>
#include <libsbx/graphics/images/image2d_array.hpp>
#include <libsbx/graphics/images/separate_image2d_array.hpp>
#include <libsbx/graphics/images/separate_sampler.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>

#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/point_light.hpp>
#include <libsbx/scenes/components/global_transform.hpp>


#include <libsbx/animation/vertex3d.hpp>
#include <libsbx/animation/pipeline.hpp>
#include <libsbx/animation/mesh.hpp>

namespace sbx::animation {

class skinned_mesh_subrenderer final : public graphics::subrenderer {

public:

  skinned_mesh_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _pipeline{path, stage},
    _push_handler{_pipeline},
    _scene_descriptor_handler{_pipeline, 0u} {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    _draw_commands_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    _transform_data_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    _instance_data_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  }

  ~skinned_mesh_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    EASY_FUNCTION();

    SBX_SCOPED_TIMER("skinned_mesh_subrenderer");

    std::this_thread::sleep_for(std::chrono::milliseconds{2});
  }

private:




  // std::unordered_map<math::uuid, std::vector<std::vector<instance_data>>> _submesh_instances;
  // std::vector<transform_data> _transform_data;

  pipeline _pipeline;

  graphics::storage_buffer_handle _draw_commands_buffer;
  graphics::storage_buffer_handle _transform_data_buffer;
  graphics::storage_buffer_handle _instance_data_buffer;

  graphics::descriptor_handler _scene_descriptor_handler;
  graphics::uniform_handler _scene_uniform_handler;
  graphics::push_handler _push_handler;

  graphics::separate_sampler _images_sampler;
  graphics::separate_image2d_array _images;

}; // class mesh_subrenderer

} // namespace sbx::animation

#endif // LIBSBX_ANIMATION_SKINNED_MESH_SUBRENDERER_HPP_
