#ifndef LIBSBX_MODELS_STATIC_MESH_DRAW_LIST_HPP_
#define LIBSBX_MODELS_STATIC_MESH_DRAW_LIST_HPP_

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
#include <libsbx/graphics/images/image2d_array.hpp>
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

namespace sbx::models {

struct transform_data {
  alignas(16) math::matrix4x4 model;
  alignas(16) math::matrix4x4 normal;
}; // struct transform_data

struct instance_data {
  alignas(16) math::color tint;
  alignas(16) math::vector4 material;
  alignas(16) math::vector4u payload; // x: albedo image index, y: normal image index, y: instance data index, w: unused
  alignas(16) math::vector4u selection; // x: upper 32 bit of id, y: lower 32 bit of id, z: unused, w: unused
}; // struct instance_data

class static_mesh_draw_list final : public graphics::draw_list {

public:

  inline static constexpr auto draw_commands_buffer_name = utility::hashed_string{"draw_commands"};
  inline static constexpr auto transform_data_buffer_name = utility::hashed_string{"transform_data"};
  inline static constexpr auto instance_data_buffer_name = utility::hashed_string{"instance_data"};

  static_mesh_draw_list() {
    create_buffer("draw_commands", graphics::storage_buffer::min_size, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    create_buffer("transform_data", graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    create_buffer("instance_data", graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  }

  auto update() -> void override {
    _submesh_instances.clear();
    _transform_data.clear();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto static_mesh_query = scene.query<const scenes::static_mesh, const scenes::global_transform>();

    for (auto&& [node, static_mesh, global_transform] : static_mesh_query.each()) {
      _submit_mesh(node, static_mesh);
    }

    _build_draw_commands();
  }

private:

  auto _submit_mesh(const scenes::node node, const scenes::static_mesh& static_mesh) -> void {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto mesh_id = static_mesh.mesh_id();

    const auto& global_transform = scene.get_component<const scenes::global_transform>(node);
    const auto selection_tag = scene.has_component<const scenes::selection_tag>(node) ? scene.get_component<const scenes::selection_tag>(node) : scenes::selection_tag::null;

    const auto upper_id = static_cast<std::uint32_t>(selection_tag.value() >> 32u);
    const auto lower_id = static_cast<std::uint32_t>(selection_tag.value() & 0xFFFFFFFF);

    const auto transform_data_index = static_cast<std::uint32_t>(_transform_data.size());
    _transform_data.emplace_back(global_transform.model, global_transform.normal);

    auto& instances = _submesh_instances[mesh_id];

    for (const auto& submesh : static_mesh.submeshes()) {
      const auto albedo_image_index = submesh.albedo_texture ? add_image(submesh.albedo_texture) : graphics::separate_image2d_array::max_size;
      const auto normal_image_index = submesh.normal_texture ? add_image(submesh.normal_texture) : graphics::separate_image2d_array::max_size;

      const auto material = math::vector4{submesh.material.metallic, submesh.material.roughness, submesh.material.flexibility, submesh.material.anchor_height};
      const auto payload = math::vector4u{albedo_image_index, normal_image_index, transform_data_index, 0u};
      const auto selection = math::vector4u{upper_id, lower_id, 0u, 0u};

      instances.resize(std::max(instances.size(), static_cast<std::size_t>(submesh.index + 1u)));
      instances[submesh.index].push_back(instance_data{submesh.tint, material, payload, selection});
    }
  }

  auto _build_draw_commands() -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto draw_commands = std::vector<VkDrawIndexedIndirectCommand>{};
    auto instance_data = std::vector<models::instance_data>{};

    auto base_instance = std::uint32_t{0u};

    for (auto&& [mesh_id, submesh] : _submesh_instances) {
      auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);

      auto range = graphics::draw_command_range{};
      range.offset = static_cast<uint32_t>(draw_commands.size());

      for (auto&& [submesh_index, instances] : ranges::views::enumerate(submesh)) {
        if (instances.empty()) {
          continue;
        }

        const auto submesh = mesh.submesh(submesh_index);

        const auto instance_count = static_cast<std::uint32_t>(instances.size());

        auto command = VkDrawIndexedIndirectCommand{};
        command.indexCount = submesh.index_count;
        command.instanceCount = instance_count;
        command.firstIndex = submesh.index_offset;
        command.vertexOffset = submesh.vertex_offset;
        command.firstInstance = base_instance;
        // command.firstInstance = 0u;

        draw_commands.push_back(command);

        utility::append(instance_data, std::move(instances));
        // instance_data.insert(instance_data.end(), instances.begin(), instances.end());

        base_instance += instance_count;
        range.count++;
      }

      if (range.count > 0) {
        push_draw_command_range(mesh_id, range);
      }
    }

    if (draw_commands.empty()) {
      utility::logger<"models">::warn("No static meshes found in scene, skipping draw command generation");
      return;
    }

    update_buffer(draw_commands, "draw_commands");
    update_buffer(_transform_data, "transform_data");
    update_buffer(instance_data, "instance_data");
  }

  std::unordered_map<math::uuid, std::vector<std::vector<instance_data>>> _submesh_instances;
  std::vector<transform_data> _transform_data;

}; // class static_mesh_draw_list

} // namespace sbx::models

#endif // LIBSBX_MODELS_STATIC_MESH_DRAW_LIST_HPP_
