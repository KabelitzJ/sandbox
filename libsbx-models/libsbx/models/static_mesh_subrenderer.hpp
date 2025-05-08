#ifndef LIBSBX_MODELS_MESH_SUBRENDERER_HPP_
#define LIBSBX_MODELS_MESH_SUBRENDERER_HPP_

#include <filesystem>
#include <unordered_set>
#include <ranges>
#include <algorithm>

#include <easy/profiler.h>

#include <fmt/format.h>

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

public:

  static_mesh_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _opaque_pipeline{fmt::format("{}_{}", path.string(), "opaque"), stage},
    _transparent_pipeline{fmt::format("{}_{}", path.string(), "transparent"), stage} { }

  ~static_mesh_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    EASY_FUNCTION();

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

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
    _scene_uniform_handler.push("point_light_count", 0u);

    _scene_uniform_handler.push("time", std::fmod(core::engine::time().value() * 0.5f, 1.0f));

    for (auto entry = _uniform_data.begin(); entry != _uniform_data.end();) {
      if (_used_uniforms.contains(entry->first)) {
        ++entry;
      } else {
        entry = _uniform_data.erase(entry);
      }
    }

    _used_uniforms.clear();
    _opaque_static_meshes.clear();
    _transparent_static_meshes.clear();
    _images.clear();

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

    _render_static_meshes(command_buffer, _opaque_pipeline, _opaque_static_meshes);

    // _opaque_pipeline.bind(command_buffer);

    // for (const auto& [key, data] : _opaque_static_meshes) {

    //   auto& uniform_data = _uniform_data[key];

    //   auto& descriptor_handler = uniform_data.descriptor_handler;
    //   auto& storage_handler = uniform_data.storage_handler;

    //   storage_handler.push(std::span<const per_mesh_data>{data});

    //   auto& mesh = graphics_module.get_asset<models::mesh>(key.mesh_id);

    //   descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    //   descriptor_handler.push("buffer_mesh_data", storage_handler);
    //   descriptor_handler.push("buffer_point_lights", _point_lights_storage_handler);
    //   descriptor_handler.push("shadow_map_image", graphics_module.attachment("shadow_map"));
    //   descriptor_handler.push("images_sampler", _images_sampler);
    //   descriptor_handler.push("images", _images);

    //   if (!descriptor_handler.update(_opaque_pipeline)) {
    //     continue;
    //   }

    //   descriptor_handler.bind_descriptors(command_buffer);

    //   mesh.render_submesh(command_buffer, key.submesh_index, static_cast<std::uint32_t>(data.size()));
    // }

    EASY_END_BLOCK;

    EASY_BLOCK("render transparent meshes");

    _render_static_meshes(command_buffer, _transparent_pipeline, _transparent_static_meshes);

    EASY_END_BLOCK
  }

private:

  struct uniform_data {
    graphics::descriptor_handler descriptor_handler;
    graphics::storage_handler storage_handler;
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

  struct point_light {
    alignas(16) math::vector3 position;
    alignas(16) math::color color;
    alignas(16) std::float_t radius;
  }; // struct point_light

  using opaque_pipeline = pipeline<false>;
  using transparent_pipeline = pipeline<true>;

  using static_mash_map = std::unordered_map<mesh_key, std::vector<per_mesh_data>, mesh_key_hash, mesh_key_equal>;

  template<bool UsesTransparency>
  auto _render_static_meshes(graphics::command_buffer& command_buffer, pipeline<UsesTransparency>& pipeline, static_mash_map& static_mashes) -> void {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    pipeline.bind(command_buffer);

    for (const auto& [key, data] : static_mashes) {

      auto& uniform_data = _uniform_data[key];

      auto& descriptor_handler = uniform_data.descriptor_handler;
      auto& storage_handler = uniform_data.storage_handler;

      storage_handler.push(std::span<const per_mesh_data>{data});

      auto& mesh = graphics_module.get_asset<models::mesh>(key.mesh_id);

      descriptor_handler.push("uniform_scene", _scene_uniform_handler);
      descriptor_handler.push("buffer_mesh_data", storage_handler);
      descriptor_handler.push("buffer_point_lights", _point_lights_storage_handler);
      descriptor_handler.push("shadow_map_image", graphics_module.attachment("shadow_map"));
      descriptor_handler.push("images_sampler", _images_sampler);
      descriptor_handler.push("images", _images);

      if (!descriptor_handler.update(pipeline)) {
        continue;
      }

      descriptor_handler.bind_descriptors(command_buffer);

      mesh.render_submesh(command_buffer, key.submesh_index, static_cast<std::uint32_t>(data.size()));
    }
  }

  auto _submit_mesh(const scenes::node node, const scenes::static_mesh& static_mesh) -> void {
    EASY_FUNCTION();
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto mesh_id = static_mesh.mesh_id();

    for (const auto& submesh : static_mesh.submeshes()) {
      EASY_BLOCK("submit submesh");
      const auto key = mesh_key{mesh_id, submesh.index};

      _used_uniforms.insert(key);

      const auto& global_transform = scene.get_component<const scenes::global_transform>(node);

      const auto albedo_image_index = submesh.albedo_texture ? _images.push_back(*submesh.albedo_texture) : graphics::separate_image2d_array::max_size;
      const auto normal_image_index = submesh.normal_texture ? _images.push_back(*submesh.normal_texture) : graphics::separate_image2d_array::max_size;

      const auto image_indices = math::vector4{albedo_image_index, normal_image_index, 0u, 0u};
      const auto material = math::vector4{submesh.material.metallic, submesh.material.roughness, submesh.material.flexibility, submesh.material.anchor_height};

      auto& map = submesh.uses_transparency ? _transparent_static_meshes : _opaque_static_meshes;

      map[key].push_back(per_mesh_data{global_transform.model, global_transform.normal, submesh.tint, material, image_indices});
      EASY_END_BLOCK;
    }
  }

  opaque_pipeline _opaque_pipeline;
  transparent_pipeline _transparent_pipeline;

  std::unordered_map<mesh_key, uniform_data, mesh_key_hash, mesh_key_equal> _uniform_data;
  std::unordered_set<mesh_key, mesh_key_hash, mesh_key_equal> _used_uniforms;

  static_mash_map _opaque_static_meshes;
  static_mash_map _transparent_static_meshes;

  graphics::uniform_handler _scene_uniform_handler;
  graphics::storage_handler _point_lights_storage_handler;

  graphics::separate_sampler _images_sampler;
  graphics::separate_image2d_array _images;

}; // class mesh_subrenderer

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_SUBRENDERER_HPP_
