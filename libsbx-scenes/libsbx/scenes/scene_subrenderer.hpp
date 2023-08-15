#ifndef LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_
#define LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_

#include <memory>

#include <libsbx/math/vector3.hpp>

#include <libsbx/models/mesh.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/transform.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/id.hpp>

namespace sbx::scenes {

class scene_subrenderer final : public graphics::subrenderer {

public:

  scene_subrenderer(const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _camera_position{2.0f, 2.0f, 1.0f},
    _light_position{-1.0f, 3.0f, 1.0f} { }

  ~scene_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& devices_module = core::engine::get_module<devices::devices_module>();
    auto& window = devices_module.window();

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::delta_time();

    _uniforms.view = math::matrix4x4::look_at(_camera_position, math::vector3{0.0f, 0.0f, 0.0f}, math::vector3::up);
    _uniforms.projection = math::matrix4x4::perspective(math::radian{45.0f}, window.aspect_ratio(), 0.1f, 100.0f);

    _scene_uniform_handler.push("view", _uniforms.view);
    _scene_uniform_handler.push("projection", _uniforms.projection);

    auto nodes = scene.query<scenes::static_mesh>();

    for (auto& node : nodes) {
      _render_node(node, command_buffer);
    }
  }

private:

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
    auto& pipeline = assets_module.get_asset<graphics::graphics_pipeline>(static_mesh.pipeline_id());

    if (pipeline.stage() != stage()) {
      return;
    }

    pipeline.bind(command_buffer);

    auto world_transform = scene.world_transform(node);

    auto& [uniform_handler, descriptor_handler] = _uniform_data[id];

    uniform_handler.push("model", world_transform);
    uniform_handler.push("normal", math::matrix4x4::transposed(math::matrix4x4::inverted(world_transform)));

    pipeline.bind(command_buffer);

    descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    descriptor_handler.push("uniform_object", uniform_handler);
    descriptor_handler.push("image", image);

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    mesh.render(command_buffer);
  }

  // _object_uniforms.push("model", model);
  // _object_uniforms.push("normal", math::matrix4x4::transposed(math::matrix4x4::inverted(model)));

  // if (pipeline.stage() != stage) {
  //   return;
  // }

  // pipeline.bind(command_buffer);

  // _descriptor_handler.push("uniform_scene", scene_uniforms);
  // _descriptor_handler.push("uniform_object", _object_uniforms);
  // _descriptor_handler.push("image", image);

  // if (!_descriptor_handler.update(pipeline)) {
  //   return;
  // }

  // _descriptor_handler.bind_descriptors(command_buffer);

  struct uniforms {
    math::matrix4x4 view;
    math::matrix4x4 projection;
  }; // struct uniforms#

  struct uniform_data {
    graphics::uniform_handler uniform_handler;
    graphics::descriptor_handler descriptor_handler;
  }; // struct uniform_data#

  math::vector3 _camera_position;
  math::vector3 _light_position;

  std::unordered_map<math::uuid, uniform_data> _uniform_data;

  uniforms _uniforms;
  graphics::uniform_handler _scene_uniform_handler;

}; // class scene_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_
