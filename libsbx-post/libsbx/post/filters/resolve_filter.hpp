#ifndef LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_
#define LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_

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
    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& pipeline = base_type::pipeline();
    auto& descriptor_handler = base_type::descriptor_handler();

    const auto& camera_transform = camera_node.get_component<math::transform>();

    _scene_uniform_handler.push("camera_position", camera_transform.position());

    const auto& scene_light = scene.light();

    const auto light_direction = scene_light.direction();

    const auto position = light_direction * -30.0f;

    const auto view = math::matrix4x4::look_at(position, position + light_direction, math::vector3::up);
    const auto projection = math::matrix4x4::orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 2000.0f);

    _scene_uniform_handler.push("light_space", math::matrix4x4{projection * view});

    _scene_uniform_handler.push("light_direction", light_direction);
    _scene_uniform_handler.push("light_color", scene_light.color());

    // auto light_nodes = scene.query<scenes::point_light>();

    // auto lights = std::vector<models::point_light>{};
    // auto point_light_count = std::uint32_t{0};

    // for (const auto& node : light_nodes) {
    //   const auto& light = node.get_component<scenes::point_light>();
    //   const auto& transform = node.get_component<math::transform>();

    //   lights.push_back(models::point_light{light.color(), transform.position(), light.radius()});
      
    //   ++point_light_count;

    //   if (point_light_count >= max_point_lights) {
    //     break;
    //   }
    // }

    // _lights_storage_handler.push(std::span<const models::point_light>{lights.data(), point_light_count});
    // _scene_uniform_handler.push("point_light_count", point_light_count);

    pipeline.bind(command_buffer);

    descriptor_handler.push("uniform_scene", _scene_uniform_handler);

    for (const auto& [name, attachment] : _attachment_names) {
      descriptor_handler.push(name, graphics_module.attachment(attachment));
    }

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
