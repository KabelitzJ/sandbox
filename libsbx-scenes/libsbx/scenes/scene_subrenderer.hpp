#ifndef LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_
#define LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_

#include <memory>

#include <libsbx/math/vector3.hpp>

#include <libsbx/models/mesh.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/scenes/scenes_module.hpp>

namespace sbx::scenes {

class scene_subrenderer final : public graphics::subrenderer {

public:

  scene_subrenderer(const graphics::pipeline::stage& stage, const std::filesystem::path& path)
  : graphics::subrenderer{stage},
    _pipeline{std::make_unique<graphics::graphics_pipeline>(stage, path, graphics::vertex_input<models::vertex3d>::description())},
    _uniforms{_pipeline->find_descriptor_block("buffer_object")} {
    auto& devices_module = core::engine::get_module<devices::devices_module>();

    auto& window = devices_module.window();

    _camera_position = math::vector3{2.0f, 2.0f, 1.0f};

    _light_position = math::vector3{-1.0f, 3.0f, 1.0f};

    _uniform_buffer_object.model = math::matrix4x4::identity;
    _uniform_buffer_object.view = math::matrix4x4::look_at(_camera_position, math::vector3{0.0f, 0.0f, 0.0f}, math::vector3::up);
    _uniform_buffer_object.projection = math::matrix4x4::perspective(math::radian{45.0f}, window.aspect_ratio(), 0.1f, 100.0f);
    _uniform_buffer_object.normal = math::matrix4x4::identity;
  }

  ~scene_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& devices_module = core::engine::get_module<devices::devices_module>();
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& window = devices_module.window();

    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& mesh = assets_module.get_asset<models::mesh>("./demo/assets/meshes/suzanne.obj");
    auto& image = assets_module.get_asset<graphics::image2d>("./demo/assets/textures/base.png");

    const auto delta_time = core::engine::delta_time();

    _pipeline->bind(command_buffer);

    _uniform_buffer_object.model = math::matrix4x4::rotated(_uniform_buffer_object.model, math::vector3::up, math::degree{45.0f} * delta_time);
    _uniform_buffer_object.projection = math::matrix4x4::perspective(math::radian{45.0f}, window.aspect_ratio(), 0.1f, 10.0f);
    _uniform_buffer_object.normal = math::matrix4x4::transposed(math::matrix4x4::inverted(_uniform_buffer_object.model));

    _uniforms.push("normal", _uniform_buffer_object.normal);
    _uniforms.push("view", _uniform_buffer_object.view);
    _uniforms.push("model", _uniform_buffer_object.model);
    _uniforms.push("projection", _uniform_buffer_object.projection);

    _pipeline->push(_uniforms);
    _pipeline->push("image", image);

    _pipeline->bind_descriptors(command_buffer);

    mesh.render(command_buffer);
  }

private:

  struct uniform_buffer_object {
    math::matrix4x4 model;
    math::matrix4x4 view;
    math::matrix4x4 projection;
    math::matrix4x4 normal;
  }; // struct uniform_buffer_object

  math::vector3 _camera_position;
  math::vector3 _light_position;

  std::unique_ptr<graphics::graphics_pipeline> _pipeline;

  graphics::uniform_handler _uniforms;
  uniform_buffer_object _uniform_buffer_object;

}; // class scene_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_SUBRENDERER_HPP_