#ifndef LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_
#define LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_

#include <memory>

#include <libsbx/math/vector3.hpp>

#include <libsbx/models/mesh.hpp>
#include <libsbx/models/vertex3d.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/scripting/script.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/transform.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/script.hpp>

namespace sbx::scenes {

class scene_subrenderer final : public graphics::subrenderer {

public:

  scene_subrenderer(const graphics::pipeline::stage& stage, const std::filesystem::path& path)
  : graphics::subrenderer{stage},
    _pipeline{stage, path, graphics::vertex_input<models::vertex3d>::description()},
    _camera_position{2.0f, 2.0f, 1.0f},
    _light_position{-1.0f, 3.0f, 1.0f} { }

  ~scene_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& devices_module = core::engine::get_module<devices::devices_module>();
    auto& window = devices_module.window();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto camera_nodes = scene.query<scenes::camera>();

    auto has_active_camera = false;

    for (auto& node : camera_nodes) {
      auto& camera = node.get_component<scenes::camera>();

      if (!camera.is_active()) {
        continue;
      }

      has_active_camera = true;

      camera.set_aspect_ratio(window.aspect_ratio());

      _scene_uniform_handler.push("projection", camera.projection());

      auto& transform = node.get_component<scenes::transform>();

      _scene_uniform_handler.push("view", math::matrix4x4::inverted(transform.as_matrix()));

      break;
    }

    if (!has_active_camera) {
      core::logger::warn("sbx::scenes", "Scene does not have an active camera");
      return;
    }

    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto script_nodes = scene.query<scenes::script>();

    for (auto& node : script_nodes) {
      auto& transform = node.get_component<scenes::transform>();

      auto& script_id = node.get_component<scenes::script>();
      auto& script = assets_module.get_asset<scripting::script>(script_id);

      script.set("position", transform.position());
      script.set("rotation", transform.rotation());

      script.on_update();

      if (auto position = script.get<math::vector3>("position"); position) {
        transform.set_position(*position);
      }

      if (auto rotation = script.get<math::vector3>("position"); rotation) {
        transform.set_rotation(*rotation);
      }
    }

    auto mesh_nodes = scene.query<scenes::static_mesh>();

    for (auto& node : mesh_nodes) {
      _render_node(node, command_buffer);
    }
  }

private:

  // auto _forward(const math::vector3& position, const math::vector3& rotation) -> math::vector3 {
  //   auto matrix = math::matrix4x4::identity;

  //   matrix = math::matrix4x4::rotated(matrix, math::vector3::right, math::degree{rotation.x});
  //   matrix = math::matrix4x4::rotated(matrix, math::vector3::forward, math::degree{rotation.y});

  //   auto rotated = matrix * math::vector4{position};

  //   return math::vector3{rotated.x, rotated.y, rotated.z};
  // }

  // auto _up(const math::vector3& position, const math::vector3& rotation) -> math::vector3 {

  // }

  auto _render_node(node& node, graphics::command_buffer& command_buffer) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::delta_time();

    const auto& static_mesh = node.get_component<scenes::static_mesh>();
    const auto& id = node.get_component<scenes::id>();
    auto& transform = node.get_component<scenes::transform>();

    transform.set_rotation(transform.rotation() + math::vector3{0.0f, 0.0f, math::degree{45.0f} * delta_time});

    auto& mesh = assets_module.get_asset<models::mesh>(static_mesh.mesh_id());
    auto& image = assets_module.get_asset<graphics::image2d>(static_mesh.texture_id());

    if (_pipeline.stage() != stage()) {
      return;
    }

    _pipeline.bind(command_buffer);

    auto world_transform = scene.world_transform(node);

    auto& [uniform_handler, descriptor_handler] = _uniform_data[id];

    uniform_handler.push("model", world_transform);
    uniform_handler.push("normal", math::matrix4x4::transposed(math::matrix4x4::inverted(world_transform)));

    descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    descriptor_handler.push("uniform_object", uniform_handler);
    descriptor_handler.push("image", image);

    if (!descriptor_handler.update(_pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    mesh.render(command_buffer);
  }

  struct uniform_data {
    graphics::uniform_handler uniform_handler;
    graphics::descriptor_handler descriptor_handler;
  }; // struct uniform_data#

  graphics::graphics_pipeline _pipeline;

  math::vector3 _camera_position;
  math::vector3 _light_position;

  std::unordered_map<math::uuid, uniform_data> _uniform_data;

  math::matrix4x4 _view;
  math::matrix4x4 _projection;

  graphics::uniform_handler _scene_uniform_handler;

}; // class scene_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_
