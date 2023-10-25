#ifndef LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_
#define LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/buffer/push_handler.hpp>
#include <libsbx/graphics/buffer/uniform_handler.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>

#include <libsbx/models/mesh.hpp>

#include <libsbx/shadows/vertex3d.hpp>
#include <libsbx/shadows/pipeline.hpp>

namespace sbx::shadows {

class shadow_subrenderer : public graphics::subrenderer {

public:

  shadow_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _pipeline{path, stage} {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    scene.on_component_added<scenes::static_mesh>() += [this](scenes::node& node){
      const auto& id = node.get_component<scenes::id>();

      _uniform_data.insert({id, std::make_unique<uniform_data>()});
    };

    scene.on_component_removed<scenes::static_mesh>() += [this](scenes::node& node){
      const auto& id = node.get_component<scenes::id>();

      _uniform_data.erase(id);
    };
  }

  ~shadow_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& camera = camera_node.get_component<scenes::camera>();

    _scene_uniform_handler.push("projection", camera.projection());

    auto& transform = camera_node.get_component<math::transform>();

    _scene_uniform_handler.push("view", math::matrix4x4::inverted(transform.as_matrix()));

    auto mesh_nodes = scene.query<scenes::static_mesh>();

    for (auto& node : mesh_nodes) {
      _render_node(node, command_buffer);
    }
  }

private:

  auto _render_node(scenes::node& node, graphics::command_buffer& command_buffer) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto& static_mesh = node.get_component<scenes::static_mesh>();
    const auto& id = node.get_component<scenes::id>();

    auto& mesh = assets_module.get_asset<models::mesh>(static_mesh.mesh_id());

    auto world_transform = scene.world_transform(node);

    auto& uniform_data = _uniform_data.at(id);
    
    auto& push_handler = uniform_data->push_handler;
    auto& descriptor_handler = uniform_data->descriptor_handler;

    _pipeline.bind(command_buffer);

    push_handler.push("model", world_transform);

    descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    descriptor_handler.push("object", push_handler);

    if (!descriptor_handler.update(_pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);
    push_handler.bind(command_buffer, _pipeline);

    mesh.render(command_buffer);
  }

  struct uniform_data {
    graphics::push_handler push_handler;
    graphics::descriptor_handler descriptor_handler;
  }; // struct uniform_data

  pipeline _pipeline;

  std::unordered_map<math::uuid, std::unique_ptr<uniform_data>> _uniform_data;

  graphics::uniform_handler _scene_uniform_handler;

}; // class shadow_subrenderer

} // namespace sbx::shadows

#endif // LIBSBX_SHADOWS_SHADOW_SUBRENDERER_HPP_
