#ifndef LIBSBX_MODELS_MESH_SUBRENDERER_HPP_
#define LIBSBX_MODELS_MESH_SUBRENDERER_HPP_

#include <filesystem>
#include <unordered_set>
#include <ranges>
#include <algorithm>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>
#include <libsbx/graphics/buffers/uniform_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/script.hpp>
#include <libsbx/scenes/components/point_light.hpp>

#include <libsbx/models/vertex3d.hpp>
#include <libsbx/models/pipeline.hpp>
#include <libsbx/models/mesh.hpp>

namespace sbx::models {

struct point_light {
  math::color color;
  math::vector3 position;
  std::float_t intensity;
}; // struct point_light

class mesh_subrenderer final : public graphics::subrenderer {

  inline static constexpr auto max_point_lights = std::size_t{16};

public:

  mesh_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _pipeline{path, stage} { }

  ~mesh_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& camera = camera_node.get_component<scenes::camera>();

    if (!camera.is_active()) {
      core::logger::warn("Scene does not have an active camera");
      return;
    }

    _scene_uniform_handler.push("projection", camera.projection());

    auto& camera_transform = camera_node.get_component<math::transform>();

    _scene_uniform_handler.push("view", math::matrix4x4::inverted(camera_transform.as_matrix()));

    const auto time = std::fmod(core::engine::time().value() * scene.wind_speed(), 1.0f);
    _scene_uniform_handler.push("time", time);

    for (auto entry = _uniform_data.begin(); entry != _uniform_data.end();) {
      if (_used_uniforms.contains(entry->first)) {
        ++entry;
      } else {
        entry = _uniform_data.erase(entry);
      }
    }

    _used_uniforms.clear();
    _static_meshes.clear();

    auto mesh_nodes = scene.query<scenes::static_mesh>();

    for (auto& node : mesh_nodes) {
      _submit_mesh(node);
    }

    for (const auto& [key, data] : _static_meshes) {
      _pipeline.bind(command_buffer);

      auto& uniform_data = _uniform_data[key];

      auto& descriptor_handler = uniform_data.descriptor_handler;
      auto& storage_handler = uniform_data.storage_handler;

      storage_handler.push(std::span<const per_mesh_data>{data});

      auto& mesh = assets_module.get_asset<models::mesh>(key.mesh_id);
      auto& image = assets_module.get_asset<graphics::image2d>(key.texture_id);

      descriptor_handler.push("uniform_scene", _scene_uniform_handler);
      descriptor_handler.push("buffer_mesh_data", storage_handler);
      descriptor_handler.push("image", image);

      if (!descriptor_handler.update(_pipeline)) {
        return;
      }

      descriptor_handler.bind_descriptors(command_buffer);

      mesh.render_submesh(command_buffer, key.submesh_index, static_cast<std::uint32_t>(data.size()));
    }
  }

private:

  auto _submit_mesh(scenes::node& node) -> void {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto& static_mesh = node.get_component<scenes::static_mesh>();
    const auto mesh_id = static_mesh.mesh_id();

    for (const auto& submesh : static_mesh.submeshes()) {
      const auto key = mesh_key{mesh_id, submesh.texture_id, submesh.index};

      _used_uniforms.insert(key);

      auto model = scene.world_transform(node);
      auto normal = math::matrix4x4::transposed(math::matrix4x4::inverted(model));

      _static_meshes[key].push_back(per_mesh_data{std::move(model), std::move(normal), submesh.tint, math::vector4{submesh.flexibility, submesh.anchor_height, 0.0f, 0.0f}});
    }
  }

  struct uniform_data {
    graphics::descriptor_handler descriptor_handler;
    graphics::storage_handler storage_handler;
  }; // struct uniform_data

  struct mesh_key {
    assets::asset_id mesh_id;
    assets::asset_id texture_id;
    std::uint32_t submesh_index;
  }; // struct mesh_key

  struct per_mesh_data {
    math::matrix4x4 model;
    math::matrix4x4 normal;
    math::color tint;
    math::vector4 wind; // x: flexibility, y: anchor_height, zw: unused
  }; // struct per_mesh_data

  struct mesh_key_hash {
    auto operator()(const mesh_key& key) const noexcept -> std::size_t {
      auto seed = std::size_t{0};

      utility::hash_combine(seed, key.mesh_id, key.texture_id, key.submesh_index);

      return seed;
    }
  }; // struct mesh_key_hash

  struct mesh_key_equal {
    auto operator()(const mesh_key& lhs, const mesh_key& rhs) const noexcept -> bool {
      return lhs.mesh_id == rhs.mesh_id && lhs.texture_id == rhs.texture_id && lhs.submesh_index == rhs.submesh_index;
    }
  }; // struct mesh_key_equal

  pipeline _pipeline;

  std::unordered_map<mesh_key, uniform_data, mesh_key_hash, mesh_key_equal> _uniform_data;
  std::unordered_set<mesh_key, mesh_key_hash, mesh_key_equal> _used_uniforms;

  std::unordered_map<mesh_key, std::vector<per_mesh_data>, mesh_key_hash, mesh_key_equal> _static_meshes;

  graphics::uniform_handler _scene_uniform_handler;
  graphics::storage_handler _lights_storage_handler;

}; // class mesh_subrenderer

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_SUBRENDERER_HPP_
