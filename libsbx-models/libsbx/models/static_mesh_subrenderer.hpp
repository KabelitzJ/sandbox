#ifndef LIBSBX_MODELS_MESH_SUBRENDERER_HPP_
#define LIBSBX_MODELS_MESH_SUBRENDERER_HPP_

#include <filesystem>
#include <unordered_set>
#include <ranges>
#include <algorithm>

#include <easy/profiler.h>

#include <fmt/format.h>

#include <tsl/robin_map.h>
#include <tsl/robin_set.h>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/memory/octtree.hpp>

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

#include <libsbx/models/vertex3d.hpp>
#include <libsbx/models/pipeline.hpp>
#include <libsbx/models/mesh.hpp>

namespace sbx::models {

class static_mesh_subrenderer final : public graphics::subrenderer {

  inline static constexpr auto max_point_lights = std::size_t{16};

  template<typename Type>
  requires (std::is_trivially_copyable_v<Type>)
  auto _specialization_info(const Type& value) -> const VkSpecializationInfo* {
    static auto specialization_map_entry = VkSpecializationMapEntry{};
    specialization_map_entry.constantID = 0u;
    specialization_map_entry.offset = 0u;
    specialization_map_entry.size = sizeof(Type);

    static auto specialization_data = Type{value};

    static auto specialization_info = VkSpecializationInfo{};
    specialization_info.mapEntryCount = 1u;
    specialization_info.pMapEntries = &specialization_map_entry;
    specialization_info.dataSize = sizeof(Type);
    specialization_info.pData = &specialization_data;

    return &specialization_info;
  }

  inline static constexpr auto transparency_disabled = std::uint32_t{0u};
  inline static constexpr auto transparency_enabled = std::uint32_t{1u};

public:

  static_mesh_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _opaque_pipeline_data{path, stage, _specialization_info(transparency_disabled)},
    _transparent_back_pipeline_data{path, stage, _specialization_info(transparency_enabled)},
    _transparent_front_pipeline_data{path, stage, _specialization_info(transparency_enabled)},
    _scene_descriptor_handler{0u} { }

  ~static_mesh_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    EASY_FUNCTION();

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

    // auto point_light_nodes = scene.query<scenes::point_light>();

    // auto point_lights = std::vector<point_light>{};
    // auto point_light_count = std::uint32_t{0};

    // for (const auto& node : point_light_nodes) {
    //   const auto model = scene.world_transform(node);

    //   const auto& light = node.get_component<scenes::point_light>();

    //   const auto position = math::vector3{model[3]};

    //   point_lights.push_back(point_light{position, light.color(), light.radius()});
      
    //   ++point_light_count;

    //   if (point_light_count >= max_point_lights) {
    //     break;
    //   }
    // }

    // _point_lights_storage_handler.push(std::span<const point_light>{point_lights.data(), point_light_count});
    // _scene_uniform_handler.push("point_light_count", 0u);

    _scene_uniform_handler.push("time", std::fmod(core::engine::time().value() * 0.5f, 1.0f));

    EASY_END_BLOCK;

    EASY_BLOCK("clearing old data");

    EASY_BLOCK("clearing _opaque_pipeline_data");
    _opaque_pipeline_data.clear();
    EASY_END_BLOCK;

    EASY_BLOCK("clearing _transparent_back_pipeline_data");
    _transparent_back_pipeline_data.clear();
    EASY_END_BLOCK;

    EASY_BLOCK("clearing _transparent_front_pipeline_data");
    _transparent_front_pipeline_data.clear();
    EASY_END_BLOCK;

    EASY_BLOCK("clearing _opaque_static_meshes");
    _opaque_static_meshes.clear();
    EASY_END_BLOCK;

    EASY_BLOCK("clearing _transparent_static_meshes");
    _transparent_static_meshes.clear();
    EASY_END_BLOCK;

    EASY_BLOCK("clearing _images");
    _images.clear();
    EASY_END_BLOCK;

    EASY_END_BLOCK;

    // EASY_BLOCK("sorting by camera distance");

    // scene.sort<math::transform>([&camera_global_transform, &scene](const auto& lhs, const auto& rhs) {
    //   return true;
    // });

    // EASY_END_BLOCK;

    EASY_BLOCK("building frustum");

    auto frustum = camera.view_frustum(view);

    EASY_END_BLOCK;

    EASY_BLOCK("submit meshes no collider");

    auto mesh_query_no_collider = scene.query<const scenes::static_mesh>(ecs::exclude<scenes::collider>);

    for (auto&& [node, static_mesh] : mesh_query_no_collider.each()) {
      _submit_mesh(node, static_mesh);
    }

    EASY_END_BLOCK;

    EASY_BLOCK("submit meshes collider");

    auto mesh_query_collider = scene.query<const scenes::static_mesh, const scenes::collider>();

    for (auto&& [node, static_mesh, collider] : mesh_query_collider.each()) {
      const auto model = scene.world_transform(node);

      EASY_BLOCK("testing frustum");
      if (frustum.intersects(model, collider)) {
        _submit_mesh(node, static_mesh);
      }
      EASY_END_BLOCK;
    }

    EASY_END_BLOCK;

    EASY_BLOCK("render opaque meshes");

    _render_static_meshes(command_buffer, _opaque_pipeline_data);

    EASY_END_BLOCK;

    // EASY_BLOCK("render transparent meshes");

    // _render_static_meshes(command_buffer, _transparent_back_pipeline_data);
    // _render_static_meshes(command_buffer, _transparent_front_pipeline_data);

    // EASY_END_BLOCK
  }

private:

  struct point_light {
    alignas(16) math::vector3 position;
    alignas(16) math::color color;
    alignas(16) std::float_t radius;
  }; // struct point_light

  struct uniform_data {
    graphics::descriptor_handler descriptor_handler;
    graphics::storage_handler storage_handler;

    uniform_data(std::uint32_t set)
    : descriptor_handler{set} { }

  }; // struct uniform_data

  struct mesh_key {
    math::uuid mesh_id;
    std::uint32_t submesh_index;
  }; // struct mesh_key

  struct per_mesh_data {
    alignas(16) math::matrix4x4 model;
    alignas(16) math::matrix4x4 normal;
    alignas(16) math::color tint;
    alignas(16) math::vector4 material;
    alignas(16) math::vector4 image_indices;
  }; // struct per_mesh_data

  struct mesh_key_hash {
    auto operator()(const mesh_key& key) const noexcept -> std::size_t {
      auto seed = std::size_t{0};

      utility::hash_combine(seed, key.mesh_id, key.submesh_index);

      return seed;
    }
  }; // struct mesh_key_hash

  struct mesh_key_equal {
    auto operator()(const mesh_key& lhs, const mesh_key& rhs) const noexcept -> bool {
      return lhs.mesh_id == rhs.mesh_id && lhs.submesh_index == rhs.submesh_index;
    }
  }; // struct mesh_key_equal

  template<typename... Args>
  using map_type = std::unordered_map<Args...>;
  // using map_type = tsl::robin_map<Args...>;

  template<typename... Args>
  using set_type = std::unordered_set<Args...>;
  // using set_type = tsl::robin_set<Args...>;

  template<bool UsesTransparency, graphics::cull_mode CullMode>
  struct per_pipeline_data {
    models::pipeline<UsesTransparency, CullMode> pipeline;
    map_type<mesh_key, static_mesh_subrenderer::uniform_data, mesh_key_hash, mesh_key_equal> uniform_data;
    set_type<mesh_key, mesh_key_hash, mesh_key_equal> used_uniforms;

    template<typename... Args>
    per_pipeline_data(Args&&... args)
    : pipeline{std::forward<Args>(args)...} { }

    auto clear() -> void {
      for (auto entry = uniform_data.begin(); entry != uniform_data.end();) {
        if (used_uniforms.contains(entry->first)) {
          ++entry;
        } else {
          entry = uniform_data.erase(entry);
        }
      }

      used_uniforms.clear();
    }

  }; // struct per_pipeline_data

  using opaque_pipeline_data = per_pipeline_data<false, graphics::cull_mode::back>;
  using transparent_front_pipeline_data = per_pipeline_data<true, graphics::cull_mode::back>;
  using transparent_back_pipeline_data = per_pipeline_data<true, graphics::cull_mode::front>;

  auto _submit_mesh(const scenes::node node, const scenes::static_mesh& static_mesh) -> void {
    EASY_FUNCTION();
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto mesh_id = static_mesh.mesh_id();

    for (const auto& submesh : static_mesh.submeshes()) {
      EASY_BLOCK("submit submesh");
      const auto key = mesh_key{mesh_id, submesh.index};

      if (submesh.uses_transparency) {
        _transparent_back_pipeline_data.used_uniforms.insert(key);
        _transparent_front_pipeline_data.used_uniforms.insert(key);
      } else {
        _opaque_pipeline_data.used_uniforms.insert(key);
      }

      const auto& global_transform = scene.get_component<const scenes::global_transform>(node);

      const auto albedo_image_index = submesh.albedo_texture ? _images.push_back(*submesh.albedo_texture) : graphics::separate_image2d_array::max_size;
      const auto normal_image_index = submesh.normal_texture ? _images.push_back(*submesh.normal_texture) : graphics::separate_image2d_array::max_size;

      const auto image_indices = math::vector4{albedo_image_index, normal_image_index, 0u, 0u};
      const auto material = math::vector4{submesh.material.metallic, submesh.material.roughness, submesh.material.flexibility, submesh.material.anchor_height};

      if (submesh.uses_transparency) {
        _transparent_static_meshes[key].push_back(per_mesh_data{global_transform.model, global_transform.normal, submesh.tint, material, image_indices});
      } else {
        _opaque_static_meshes[key].push_back(per_mesh_data{global_transform.model, global_transform.normal, submesh.tint, material, image_indices});
      }

      EASY_END_BLOCK;
    }
  }

  template<bool UsesTransparency, graphics::cull_mode CullMode>
  auto _render_static_meshes(graphics::command_buffer& command_buffer, per_pipeline_data<UsesTransparency, CullMode>& per_pipeline_data) -> void {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    per_pipeline_data.pipeline.bind(command_buffer);

    _scene_descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    // _scene_descriptor_handler.push("buffer_point_lights", _point_lights_storage_handler);
    // _scene_descriptor_handler.push("shadow_map_image", graphics_module.attachment("shadow_map"));
    _scene_descriptor_handler.push("images_sampler", _images_sampler);
    _scene_descriptor_handler.push("images", _images);

    if (!_scene_descriptor_handler.update(per_pipeline_data.pipeline)) {
      return;
    }

    _scene_descriptor_handler.bind_descriptors(command_buffer);

    const auto& static_meshes = UsesTransparency ? _transparent_static_meshes : _opaque_static_meshes;

    for (const auto& [key, data] : static_meshes) {
      // auto& uniform_data = per_pipeline_data.uniform_data[key];

      auto [entry, inserted] = per_pipeline_data.uniform_data.try_emplace(key, 1u);

      auto& descriptor_handler = entry->second.descriptor_handler;
      auto& storage_handler = entry->second.storage_handler;

      storage_handler.push(std::span<const per_mesh_data>{data});

      auto& mesh = graphics_module.get_asset<models::mesh>(key.mesh_id);

      descriptor_handler.push("buffer_mesh_data", storage_handler);

      if (!descriptor_handler.update(per_pipeline_data.pipeline)) {
        continue;
      }

      descriptor_handler.bind_descriptors(command_buffer);

      mesh.render_submesh(command_buffer, key.submesh_index, static_cast<std::uint32_t>(data.size()));
    }
  }

  map_type<mesh_key, std::vector<per_mesh_data>, mesh_key_hash, mesh_key_equal> _opaque_static_meshes;
  map_type<mesh_key, std::vector<per_mesh_data>, mesh_key_hash, mesh_key_equal> _transparent_static_meshes;

  // tsl::robin_map

  opaque_pipeline_data _opaque_pipeline_data;
  transparent_front_pipeline_data _transparent_front_pipeline_data;
  transparent_back_pipeline_data _transparent_back_pipeline_data;

  graphics::uniform_handler _scene_uniform_handler;
  // graphics::storage_handler _point_lights_storage_handler;
  graphics::separate_sampler _images_sampler;
  graphics::separate_image2d_array _images;
  graphics::descriptor_handler _scene_descriptor_handler;

}; // class mesh_subrenderer

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_SUBRENDERER_HPP_
