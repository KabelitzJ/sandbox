#ifndef LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_
#define LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_

#include <unordered_map>
#include <algorithm>

#include <libsbx/utility/fast_mod.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>

#include <libsbx/devices/devices_module.hpp>
#include <libsbx/devices/window.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/images/image.hpp>
#include <libsbx/graphics/images/separate_sampler.hpp>
#include <libsbx/graphics/images/separate_image2d_array.hpp>
#include <libsbx/graphics/images/depth_image.hpp>

#include <libsbx/graphics/buffers/push_handler.hpp>

#include <libsbx/graphics/buffers/uniform_handler.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/directional_light.hpp>
#include <libsbx/scenes/components/tag.hpp>

#include <libsbx/models/mesh.hpp>
#include <libsbx/models/vertex3d.hpp>

// #include <libsbx/shadows/vertex3d.hpp>
#include <libsbx/shadows/pipeline.hpp>

namespace sbx::shadows {

class shadow_subrenderer : public graphics::subrenderer {

public:

  shadow_subrenderer(const std::filesystem::path& path, const graphics::render_graph::pass& pass)
  : graphics::subrenderer{stage},
    _pipeline{path, stage} { }

  ~shadow_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& assets_module = core::engine::get_module<assets::assets_module>();
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& scene = scenes_module.scene();

    _scene_uniform_handler.push("light_space", scene.light_space());

    _scene_uniform_handler.push("time", std::fmod(core::engine::time().value() * 0.5f, 1.0f));

    for (auto entry = _uniform_data.begin(); entry != _uniform_data.end();) {
      if (_used_uniforms.contains(entry->first)) {
        ++entry;
      } else {
        entry = _uniform_data.erase(entry);
      }
    }

    _used_uniforms.clear();
    _static_meshes.clear();
    _images.clear();

    auto mesh_query = scene.query<scenes::static_mesh>();

    for (const auto node : mesh_query) {
      _submit_mesh(node);
    }

    _pipeline.bind(command_buffer);
    
    for (const auto& [key, data] : _static_meshes) {

      auto [entry, inserted] = _uniform_data.try_emplace(key, 1u);

      auto& descriptor_handler = entry->second.descriptor_handler;
      auto& storage_handler = entry->second.storage_handler;

      storage_handler.push(std::span<const per_mesh_data>{data});

      auto& mesh = assets_module.get_asset<models::mesh>(key.mesh_id);

      descriptor_handler.push("uniform_scene", _scene_uniform_handler);
      descriptor_handler.push("buffer_mesh_data", storage_handler);
      descriptor_handler.push("images_sampler", _images_sampler);
      descriptor_handler.push("images", _images);


      if (!descriptor_handler.update(_pipeline)) {
        continue;
      }

      descriptor_handler.bind_descriptors(command_buffer);

      mesh.bind(command_buffer);
      mesh.render_submesh(command_buffer, key.submesh_index, static_cast<std::uint32_t>(data.size()));
    }
  }

private:

  auto _submit_mesh(const scenes::node node) -> void {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto& static_mesh = scene.get_component<scenes::static_mesh>(node);
    const auto mesh_id = static_mesh.mesh_id();

    for (const auto& submesh : static_mesh.submeshes()) {
      const auto key = mesh_key{mesh_id, submesh.index};

      _used_uniforms.insert(key);

      auto model = scene.world_transform(node);
      
      const auto albedo_image_index = submesh.albedo_texture ? _images.push_back(submesh.albedo_texture) : graphics::separate_image2d_array::max_size;

      const auto image_indices = math::vector4{albedo_image_index, 0u, 0u, 0u};
      auto material = math::vector4{submesh.material.metallic, submesh.material.roughness, submesh.material.flexibility, submesh.material.anchor_height};

      _static_meshes[key].push_back(per_mesh_data{std::move(model), material, image_indices});
    }
  }

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

  pipeline _pipeline;

  std::unordered_map<mesh_key, uniform_data, mesh_key_hash, mesh_key_equal> _uniform_data;
  std::unordered_set<mesh_key, mesh_key_hash, mesh_key_equal> _used_uniforms;

  std::unordered_map<mesh_key, std::vector<per_mesh_data>, mesh_key_hash, mesh_key_equal> _static_meshes;

  graphics::uniform_handler _scene_uniform_handler;

  graphics::separate_sampler _images_sampler;
  graphics::separate_image2d_array _images;

}; // class shadow_subrenderer

} // namespace sbx::shadows

#endif // LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_
