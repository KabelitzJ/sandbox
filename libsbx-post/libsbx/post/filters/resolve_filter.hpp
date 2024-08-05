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

  inline static constexpr auto max_point_lights = std::size_t{32};

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

    auto& scene_light = scene.light();

    _scene_uniform_handler.push("light_space", scene.light_space());

    _scene_uniform_handler.push("light_direction", scene_light.direction());
    _scene_uniform_handler.push("light_color", scene_light.color());

    auto point_light_nodes = scene.query<scenes::point_light>();

    auto point_lights = std::vector<point_light>{};
    auto point_light_count = std::uint32_t{0};

    for (const auto& node : point_light_nodes) {
      const auto& light = node.get_component<scenes::point_light>();
      const auto& transform = node.get_component<math::transform>();

      point_lights.push_back(point_light{light.color(), transform.position(), light.radius()});
      
      ++point_light_count;

      if (point_light_count >= max_point_lights) {
        break;
      }
    }

    _point_lights_storage_handler.push(std::span<const point_light>{point_lights.data(), point_light_count});
    _scene_uniform_handler.push("point_light_count", point_light_count);

    pipeline.bind(command_buffer);

    descriptor_handler.push("uniform_scene", _scene_uniform_handler);
    descriptor_handler.push("buffer_point_lights", _point_lights_storage_handler);

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

  struct point_light {
  math::color color;
  math::vector3 position;
  std::float_t _radius;
  std::float_t intensity;
}; // struct point_light

  std::unordered_map<std::string, std::string> _attachment_names;

  graphics::uniform_handler _scene_uniform_handler;
  graphics::storage_handler _point_lights_storage_handler;

}; // class resolve_filter

} // namespace sbx::post

#endif // LIBSBX_POST_FILTERS_RESOLVE_FILTER_HPP_
