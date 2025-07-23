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
#include <libsbx/models/pipeline.hpp>
#include <libsbx/models/mesh.hpp>

namespace sbx::models {

class static_mesh_subrenderer final : public graphics::subrenderer {

  // inline static constexpr auto max_point_lights = std::size_t{16};

  // template<typename Type>
  // requires (std::is_trivially_copyable_v<Type>)
  // auto _specialization_info(const Type& value) -> const VkSpecializationInfo* {
  //   static auto specialization_map_entry = VkSpecializationMapEntry{};
  //   specialization_map_entry.constantID = 0u;
  //   specialization_map_entry.offset = 0u;
  //   specialization_map_entry.size = sizeof(Type);

  //   static auto specialization_data = Type{value};

  //   static auto specialization_info = VkSpecializationInfo{};
  //   specialization_info.mapEntryCount = 1u;
  //   specialization_info.pMapEntries = &specialization_map_entry;
  //   specialization_info.dataSize = sizeof(Type);
  //   specialization_info.pData = &specialization_data;

  //   return &specialization_info;
  // }

  // inline static constexpr auto transparency_disabled = std::uint32_t{0u};
  // inline static constexpr auto transparency_enabled = std::uint32_t{1u};

public:

  static_mesh_subrenderer(const std::filesystem::path& path, const graphics::render_graph::pass& pass)
  : graphics::subrenderer{pass},
    _pipeline{path, pass},
    _push_handler{_pipeline},
    _scene_descriptor_handler{_pipeline, 0u} {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    _draw_commands_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    _transform_data_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    _instance_data_buffer = graphics_module.add_resource<graphics::storage_buffer>(graphics::storage_buffer::min_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  }

  ~static_mesh_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    EASY_FUNCTION();

    SBX_SCOPED_TIMER("static_mesh_subrenderer");

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    EASY_BLOCK("gathering scene data");

    auto camera_node = scene.camera();

    auto& camera = scene.get_component<scenes::camera>(camera_node);

    const auto& projection = camera.projection();

    _scene_uniform_handler.push("projection", projection);

    const auto& camera_transform = scene.get_component<math::transform>(camera_node);
    const auto& camera_global_transform = scene.get_component<scenes::global_transform>(camera_node);

    const auto view = math::matrix4x4::inverted(camera_global_transform.model);

    _scene_uniform_handler.push("view", view);

    _scene_uniform_handler.push("camera_position", camera_transform.position());

    const auto& scene_light = scene.light();

    _scene_uniform_handler.push("light_space", scene.light_space());

    _scene_uniform_handler.push("light_direction", sbx::math::vector3::normalized(scene_light.direction()));
    _scene_uniform_handler.push("light_color", scene_light.color());

    _scene_uniform_handler.push("time", core::engine::time().value());

    EASY_END_BLOCK;

    EASY_BLOCK("clearing old data");

    _submesh_instances.clear();
    _transform_data.clear();
    _images.clear();

    EASY_END_BLOCK;

    EASY_BLOCK("building frustum");

    auto frustum = camera.view_frustum(view);

    EASY_END_BLOCK;

    EASY_BLOCK("submit meshes");

    // struct cull_data {
    //   scenes::node node;
    //   memory::observer_ptr<const scenes::static_mesh> static_mesh;
    //   memory::observer_ptr<const math::matrix4x4> model;
    // }; // struct cull_data

    // [NOTE] KAJ 2025-06-04 : Submitting all static meshes
    SBX_SCOPED_TIMER_BLOCK("static_mesh_subrenderer::submit") {
      auto mesh_query = scene.query<const scenes::static_mesh, const scenes::global_transform>();

      for (auto&& [node, static_mesh, global_transform] : mesh_query.each()) {
        // const auto mesh_id = static_mesh.mesh_id();
        // const auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);

        // ++total;

        // if (frustum.intersects(global_transform.model, mesh.bounds())) {
        //   ++after_cull;
        // }
        _submit_mesh(node, static_mesh);
      }

      // auto tree = containers::octree<cull_data>{math::volume{math::vector3{-500.0f}, math::vector3{500.0f}}};

      // SBX_SCOPED_TIMER_BLOCK("static_mesh_subrenderer::build_tree") {
      //   for (auto&& [node, static_mesh, global_transform] : mesh_query.each()) {
      //     const auto mesh_id = static_mesh.mesh_id();
      //     const auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);

      //     tree.insert(cull_data{node, memory::make_observer(static_mesh),  memory::make_observer(global_transform.model)}, mesh.bounds());
      //   }
      // }

      // SBX_SCOPED_TIMER_BLOCK("static_mesh_subrenderer::culling") {
      //   for (const auto& entry : tree.inside(frustum)) {
      //     if (frustum.intersects(*entry.value.model, entry.bounds)) {
      //       _submit_mesh(entry.value.node, *entry.value.static_mesh);
      //     }
      //   }
      // }
    }

    EASY_END_BLOCK;

    EASY_BLOCK("render opaque meshes");

    // [NOTE] KAJ 2025-06-04 : Rendering all static meshes
    SBX_SCOPED_TIMER_BLOCK("static_mesh_subrenderer::render"){
      _render_static_meshes(command_buffer);
    }

    EASY_END_BLOCK;
  }

private:

  struct point_light {
    alignas(16) math::vector3 position;
    alignas(16) math::color color;
    alignas(16) std::float_t radius;
  }; // struct point_light

  struct transform_data {
    alignas(16) math::matrix4x4 model;
    alignas(16) math::matrix4x4 normal;
  }; // struct transform_data

  static_assert(utility::layout_requirements_v<transform_data, 128u, 16u>, "transform_data does not meet layout requirements");

  struct instance_data {
    alignas(16) math::color tint;
    alignas(16) math::vector4 material;
    alignas(16) math::vector4u payload; // x: albedo image index, y: normal image index, y: instance data index, w: bone matrices offset
    alignas(16) math::vector4u selection;
  }; // struct instance_data

  static_assert(utility::layout_requirements_v<instance_data, 64u, 16u>, "instance_data does not meet layout requirements");

  struct draw_command_range {
    std::uint32_t offset;
    std::uint32_t count;
  }; // struct draw_command_range

  auto _submit_mesh(const scenes::node node, const scenes::static_mesh& static_mesh) -> void {
    EASY_FUNCTION();
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
      EASY_BLOCK("submit submesh");

      const auto albedo_image_index = submesh.albedo_texture ? _images.push_back(submesh.albedo_texture) : graphics::separate_image2d_array::max_size;
      const auto normal_image_index = submesh.normal_texture ? _images.push_back(submesh.normal_texture) : graphics::separate_image2d_array::max_size;

      const auto material = math::vector4{submesh.material.metallic, submesh.material.roughness, submesh.material.flexibility, submesh.material.anchor_height};
      const auto payload = math::vector4u{albedo_image_index, normal_image_index, transform_data_index, 0u};
      const auto selection = math::vector4u{upper_id, lower_id, 0u, 0u};

      instances.resize(std::max(instances.size(), static_cast<std::size_t>(submesh.index + 1u)));
      instances[submesh.index].push_back(instance_data{submesh.tint, material, payload, selection});

      EASY_END_BLOCK;
    }
  }

  auto _render_static_meshes(graphics::command_buffer& command_buffer) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    _pipeline.bind(command_buffer);

    _scene_descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    // _scene_descriptor_handler.push("buffer_point_lights", _point_lights_storage_handler);
    // _scene_descriptor_handler.push("shadow_map_image", graphics_module.attachment("shadow_map"));
    _scene_descriptor_handler.push("images_sampler", _images_sampler);
    _scene_descriptor_handler.push("images", _images);

    if (!_scene_descriptor_handler.update(_pipeline)) {
      return;
    }

    _scene_descriptor_handler.bind_descriptors(command_buffer);

    auto draw_commands = std::vector<VkDrawIndexedIndirectCommand>{};
    auto instance_data = std::vector<static_mesh_subrenderer::instance_data>{};
    auto draw_ranges = std::unordered_map<math::uuid, draw_command_range>{};

    auto base_instance = std::uint32_t{0u};

    EASY_BLOCK("build draw commands");

    for (auto&& [mesh_id, submesh] : _submesh_instances) {
      auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);

      auto range = draw_command_range{};
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
        draw_ranges.emplace(mesh_id, range);
      }
    }

    EASY_END_BLOCK;

    if (draw_commands.empty()) {
      return;
    }

    EASY_BLOCK("upload draw commands");

    // Resize and update the draw commands buffer
    auto& draw_commands_buffer = graphics_module.get_resource<graphics::storage_buffer>(_draw_commands_buffer);
    update_buffer(draw_commands, draw_commands_buffer);

    // Resize and update the transform data buffer
    auto& transform_data_buffer = graphics_module.get_resource<graphics::storage_buffer>(_transform_data_buffer);
    update_buffer(_transform_data, transform_data_buffer);

    // Resize and update the instance data buffer
    auto& instance_data_buffer = graphics_module.get_resource<graphics::storage_buffer>(_instance_data_buffer);
    update_buffer(instance_data, instance_data_buffer);

    _push_handler.push("transform_data_buffer", transform_data_buffer.address());
    _push_handler.push("instance_data_buffer", instance_data_buffer.address());

    for (const auto& [mesh_id, range] : draw_ranges) {
      auto& mesh = assets_module.get_asset<models::mesh>(mesh_id);
      
      mesh.bind(command_buffer);
      
      _push_handler.push("vertex_buffer", mesh.address());

      _push_handler.bind(command_buffer);

      command_buffer.draw_indexed_indirect(draw_commands_buffer, range.offset, range.count);
    }

    EASY_END_BLOCK;
  }

  template<typename Type>
  static auto update_buffer(const std::vector<Type>& buffer, graphics::storage_buffer& storage_buffer) -> void {
    const auto required_size = static_cast<std::uint32_t>(buffer.size() * sizeof(Type));

    if (storage_buffer.size() < required_size) {
      storage_buffer.resize(static_cast<std::size_t>(static_cast<std::float_t>(required_size) * 1.5f));
    }

    storage_buffer.update(buffer.data(), required_size);
  }

  std::unordered_map<math::uuid, std::vector<std::vector<instance_data>>> _submesh_instances;
  std::vector<transform_data> _transform_data;

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

} // namespace sbx::models

#endif // LIBSBX_MODELS_STATIC_MESH_SUBRENDERER_HPP_
