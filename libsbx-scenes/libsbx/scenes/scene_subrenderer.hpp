#ifndef LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_
#define LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_

#include <memory>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>

#include <libsbx/models/mesh.hpp>
#include <libsbx/models/vertex3d.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>
#include <libsbx/graphics/buffer/push_handler.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/models/pipeline.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/script.hpp>

namespace sbx::scenes {

class scene_subrenderer final : public graphics::subrenderer {

public:

  scene_subrenderer(const graphics::pipeline::stage& stage, const std::filesystem::path& path)
  : graphics::subrenderer{stage},
    _pipeline{stage, path},
    _camera_position{2.0f, 2.0f, 1.0f},
    _light_position{-1.0f, 3.0f, 1.0f} {
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

  ~scene_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& devices_module = core::engine::get_module<devices::devices_module>();
    auto& window = devices_module.window();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto script_nodes = scene.query<scenes::script>();

    for (auto& node : script_nodes) {
      _update_script(node);
    }

    auto camera_node = scene.camera();

    auto& camera = camera_node.get_component<scenes::camera>();

    if (!camera.is_active()) {
      core::logger::warn("Scene does not have an active camera");
      return;
    }

    camera.set_aspect_ratio(window.aspect_ratio());

    _scene_uniform_handler.push("projection", camera.projection());

    auto& transform = camera_node.get_component<math::transform>();

    _scene_uniform_handler.push("view", math::matrix4x4::inverted(transform.as_matrix()));

    _scene_uniform_handler.push("camera_position", transform.position()); 

    auto mesh_nodes = scene.query<scenes::static_mesh>();

    for (auto& node : mesh_nodes) {
      _render_node(node, command_buffer);
    }
  }

private:

  auto _update_script(node& node) -> void {
    auto& transform = node.get_component<math::transform>();

    auto& script = node.get_component<scenes::script>();

    script.set("transform", transform);

    script.invoke("on_update");

    transform = script.get<math::transform>("transform");
  }

  auto _render_node(node& node, graphics::command_buffer& command_buffer) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    const auto& static_mesh = node.get_component<scenes::static_mesh>();
    const auto& id = node.get_component<scenes::id>();

    auto& mesh = assets_module.get_asset<models::mesh>(static_mesh.mesh_id());
    auto& image = assets_module.get_asset<graphics::image2d>(static_mesh.texture_id());

    _pipeline.bind(command_buffer);

    auto world_transform = scene.world_transform(node);

    auto& uniform_data = _uniform_data.at(id);
    
    auto& push_handler = uniform_data->push_handler;
    auto& descriptor_handler = uniform_data->descriptor_handler;

    push_handler.push("model", world_transform);
    push_handler.push("normal", math::matrix4x4::transposed(math::matrix4x4::inverted(world_transform)));

    descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    descriptor_handler.push("object", push_handler);
    descriptor_handler.push("image", image);

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

  models::pipeline _pipeline;

  math::vector3 _camera_position;
  math::vector3 _light_position;

  std::unordered_map<math::uuid, std::unique_ptr<uniform_data>> _uniform_data;

  math::matrix4x4 _view;
  math::matrix4x4 _projection;

  graphics::uniform_handler _scene_uniform_handler;

}; // class scene_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_
