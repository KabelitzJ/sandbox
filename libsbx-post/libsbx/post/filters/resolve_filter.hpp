#ifndef LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_
#define LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_

#include <string>
#include <unordered_map>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/point_light.hpp>

#include <libsbx/post/filter.hpp>

namespace sbx::post {

template<graphics::vertex Vertex>
class resolve_filter final : public filter<Vertex> {

  using base_type = filter<Vertex>;

public:

  using vertex_type = base_type::vertex_type;
  using pipeline_type = base_type::pipeline_type;

  resolve_filter(const std::filesystem::path& path, const graphics::pipeline::stage& stage, std::unordered_map<std::string, std::string>&& attachment_names)
  : base_type{path, stage},
    _attachment_names{std::move(attachment_names)} { }

  ~resolve_filter() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& pipeline = base_type::pipeline();
    auto& descriptor_handler = base_type::descriptor_handler();

    auto& scene = scenes_module.scene();

    const auto camera_node = scene.camera();

    const auto& camera = camera_node.get_component<scenes::camera>();

    if (!camera.is_active()) {
      core::logger::warn("Scene does not have an active camera");
      return;
    }

    auto& camera_transform = camera_node.get_component<math::transform>();

    _scene_uniform_handler.push("camera_position", camera_transform.position()); 

    auto& scene_light = scene.light();

    auto& light_direction = scene_light.direction();
    auto& light_color = scene_light.color();

    _scene_uniform_handler.push("light_direction", light_direction);
    _scene_uniform_handler.push("light_color", light_color);

    pipeline.bind(command_buffer);

    for (const auto& [name, attachment] : _attachment_names) {
      descriptor_handler.push(name, graphics_module.attachment(attachment));
    }

    descriptor_handler.push("uniform_scene", _scene_uniform_handler);

    if (!descriptor_handler.update(pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    command_buffer.draw(6, 1, 0, 0);
  }

private:

  std::unordered_map<std::string, std::string> _attachment_names;

  graphics::uniform_handler _scene_uniform_handler;

}; // class resolve_filter

} // namespace sbx::post

#endif // LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_
