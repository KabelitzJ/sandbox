#ifndef LIBSBX_SCENES_SCENE_MODULE_HPP_
#define LIBSBX_SCENES_SCENE_MODULE_HPP_

#include <memory>
#include <optional>
#include <utility>
#include <filesystem>

#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/scenes/scene.hpp>

namespace sbx::scenes {

class scenes_module final : public core::module<scenes_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::normal);

public:

  struct line {
    sbx::math::vector4 position;
    sbx::math::color color;
  }; // struct line

  scenes_module();

  ~scenes_module() override;

  auto update() -> void override;

  auto load_scene(const std::filesystem::path& path) -> scenes::scene&;

  auto scene() -> scenes::scene&;

  auto debug_lines() const -> const std::vector<line>& {
    return _debug_lines;
  }

  auto clear_debug_lines() -> void {
    _debug_lines.clear();
  }

  auto add_debug_line(const sbx::math::vector3& start, const sbx::math::vector3& end, const sbx::math::color& color) -> void {
    _debug_lines.push_back(line{
      .position = sbx::math::vector4{start, 1.0f},
      .color = color
    });

    _debug_lines.push_back(line{
      .position = sbx::math::vector4{end, 1.0f},
      .color = color
    });
  }

  auto add_debug_plane(const sbx::math::vector3& origin, const sbx::math::vector3& v1, const sbx::math::vector3& v2, std::uint32_t n1, std::uint32_t n2, std::float_t s1, std::float_t s2, const sbx::math::color& color, const sbx::math::color& outline) -> void {
    add_debug_line(origin - s1 / 2.0f * v1 - s2 / 2.0f * v2, origin - s1 / 2.0f * v1 + s2 / 2.0f * v2, outline);
    add_debug_line(origin + s1 / 2.0f * v1 - s2 / 2.0f * v2, origin + s1 / 2.0f * v1 + s2 / 2.0f * v2, outline);
    add_debug_line(origin - s1 / 2.0f * v1 + s2 / 2.0f * v2, origin + s1 / 2.0f * v1 + s2 / 2.0f * v2, outline);
    add_debug_line(origin - s1 / 2.0f * v1 - s2 / 2.0f * v2, origin + s1 / 2.0f * v1 - s2 / 2.0f * v2, outline);

    for (int i = 1; i < n1; i++) {
      const auto t = ((std::float_t)i - (std::float_t)n1 / 2.0f) * s1/(std::float_t)n1;
      const auto o1 = origin + t * v1;
      add_debug_line(o1 - s2 / 2.0f * v2, o1 + s2 / 2.0f * v2, color);
    }

    for (int i = 1; i < n2; i++) {
      const auto t = ((std::float_t)i - (std::float_t)n2 / 2.0f) * s2/(std::float_t)n2;
      const auto o2 = origin + t * v2;
      add_debug_line(o2 - s1 / 2.0f * v1, o2 + s1 / 2.0f * v1, color);
    }
  }

  auto add_debug_volume(const math::matrix4x4& matrix, const math::volume& volume, const sbx::math::color& color) -> void {
    const auto transformed = math::volume::transformed(volume, matrix);

    const auto corners = transformed.corners();

    add_debug_line(corners[0], corners[1], color);
    add_debug_line(corners[2], corners[3], color);
    add_debug_line(corners[4], corners[5], color);
    add_debug_line(corners[6], corners[7], color);
    add_debug_line(corners[0], corners[2], color);
    add_debug_line(corners[1], corners[3], color);
    add_debug_line(corners[4], corners[6], color);
    add_debug_line(corners[5], corners[7], color);
    add_debug_line(corners[0], corners[4], color);
    add_debug_line(corners[1], corners[5], color);
    add_debug_line(corners[2], corners[6], color);
    add_debug_line(corners[3], corners[7], color);
  }

private:

  std::optional<scenes::scene> _scene;

  std::vector<line> _debug_lines{};

}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
