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

  auto add_coordinate_arrows(const math::matrix4x4& transform, std::float_t length = 2.0f, std::float_t tip_size = 0.2f) -> void {
    const auto origin = math::vector3{transform[3]};

    const auto x_axis = math::vector3::normalized(transform[0]);
    const auto y_axis = math::vector3::normalized(transform[1]);
    const auto z_axis = math::vector3::normalized(transform[2]);

    add_debug_line(origin, origin + x_axis * length, math::color::red());
    add_debug_line(origin, origin + y_axis * length, math::color::green());
    add_debug_line(origin, origin + z_axis * length, math::color::blue());
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

  auto add_debug_frustum(const math::matrix4x4& view, const math::matrix4x4& projection, const sbx::math::color& color) -> void {
    const auto corners = std::array<sbx::math::vector3, 8u>{ 
      math::vector3(-1, -1, -1),
      math::vector3(+1, -1, -1),
      math::vector3(+1, +1, -1),
      math::vector3(-1, +1, -1),
      math::vector3(-1, -1, +1),
      math::vector3(+1, -1, +1),
      math::vector3(+1, +1, +1),
      math::vector3(-1, +1, +1)
    };

    auto points = std::array<sbx::math::vector3, 8u>{};

    for (auto i = 0u; i < 8u; ++i) {
      auto q = math::matrix4x4::inverted(view) * math::matrix4x4::inverted(projection) * math::vector4{corners[i]};
      points[i] = math::vector3{q.x() / q.w(), q.y() / q.w(), q.z() / q.w()};
    }

    add_debug_line(points[0], points[4], color);
    add_debug_line(points[1], points[5], color);
    add_debug_line(points[2], points[6], color);
    add_debug_line(points[3], points[7], color);

    add_debug_line(points[0], points[1], color);
    add_debug_line(points[1], points[2], color);
    add_debug_line(points[2], points[3], color);
    add_debug_line(points[3], points[0], color);
    add_debug_line(points[0], points[2], color);
    add_debug_line(points[1], points[3], color);

    add_debug_line(points[4], points[5], color);
    add_debug_line(points[5], points[6], color);
    add_debug_line(points[6], points[7], color);
    add_debug_line(points[7], points[4], color);
    add_debug_line(points[4], points[6], color);
    add_debug_line(points[5], points[7], color);

    const auto grid_color = color * 0.7f;
    const int grid_lines = 100;

    auto p1 = points[0];
    auto p2 = points[1];
    auto s1 = (points[4]-points[0]) / static_cast<std::float_t>(grid_lines);
    auto s2 = (points[5]-points[1]) / static_cast<std::float_t>(grid_lines);

    for (int i = 0; i != grid_lines; i++, p1 += s1, p2 += s2) {
      add_debug_line(p1, p2, grid_color);
    }

    p1 = points[2];
    p2 = points[3];

    s1 = (points[6]-points[2]) / static_cast<std::float_t>(grid_lines);
    s2 = (points[7]-points[3]) / static_cast<std::float_t>(grid_lines);

    for (int i = 0; i != grid_lines; i++, p1 += s1, p2 += s2) {
      add_debug_line(p1, p2, grid_color);
    }

    p1 = points[0];
    p2 = points[3];

    s1 = (points[4]-points[0]) / static_cast<std::float_t>(grid_lines);
    s2 = (points[7]-points[3]) / static_cast<std::float_t>(grid_lines);

    for (int i = 0; i != grid_lines; i++, p1 += s1, p2 += s2) {
      add_debug_line(p1, p2, grid_color);
    }

    p1 = points[1];
    p2 = points[2];

    s1 = (points[5]-points[1]) / static_cast<std::float_t>(grid_lines);
    s2 = (points[6]-points[2]) / static_cast<std::float_t>(grid_lines);

    for (int i = 0; i != grid_lines; i++, p1 += s1, p2 += s2) {
      add_debug_line(p1, p2, grid_color);
    }
  }

private:

  std::optional<scenes::scene> _scene;

  std::vector<line> _debug_lines{};

}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
