#ifndef LIBSBX_POST_FILTERS_COMPOSE_FILTER_HPP_
#define LIBSBX_POST_FILTERS_COMPOSE_FILTER_HPP_

#include <string>
#include <unordered_map>

#include <libsbx/math/transform.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/buffers/uniform_buffer.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/components/camera.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/point_light.hpp>

#include <libsbx/post/filter.hpp>

namespace sbx::post {

class compose_filter final : public filter {

  using base_type = filter;

public:

  compose_filter(const std::filesystem::path& path, const graphics::render_graph::graphics_pass& pass, std::vector<std::pair<std::string, std::string>>&& attachment_names)
  : base_type{path, pass},
    _attachment_names{std::move(attachment_names)} { }

  ~compose_filter() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& pipeline = base_type::pipeline();
    auto& descriptor_handler = base_type::descriptor_handler();

    pipeline.bind(command_buffer);

    const auto& camera_transform = scene.get_component<math::transform>(camera_node);

    _scene_uniform_handler.push("camera_position", camera_transform.position());

    auto& scene_light = scene.light();

    _scene_uniform_handler.push("light_space", scene.light_space());

    _scene_uniform_handler.push("light_direction", sbx::math::vector3::normalized(scene_light.direction()));
    _scene_uniform_handler.push("light_color", scene_light.color());


    descriptor_handler.push("uniform_scene", _scene_uniform_handler);

    for (const auto& [name, attachment] : _attachment_names) {
      descriptor_handler.push(name, graphics_module.attachment(attachment));
    }

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    command_buffer.draw(3, 1, 0, 0);
  }

private:

  std::vector<std::pair<std::string, std::string>> _attachment_names;

  graphics::uniform_handler _scene_uniform_handler;

}; // class compose_filter

} // namespace sbx::post

#endif // LIBSBX_POST_FILTERS_COMPOSE_FILTER_HPP_
