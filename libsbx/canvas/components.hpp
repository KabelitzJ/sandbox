// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_CANVAS_COMPONENTS_HPP_
#define LIBSBX_CANVAS_COMPONENTS_HPP_

#include <cstdint>
#include <string>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>
#include <libsbx/math/uuid.hpp>

#include <libsbx/assets/texture.hpp>
#include <libsbx/assets/font.hpp>

#include <libsbx/reflection/annotations.hpp>

namespace sbx::canvas {

enum class [[=reflection::named]] render_mode : std::uint8_t {
  screen_space_overlay,
  screen_space_camera,
  world_space,
}; // enum class render_mode

enum class [[=reflection::named]] text_align : std::uint8_t {
  start,
  center,
  end,
}; // enum class text_align

struct canvas {
  render_mode mode{render_mode::screen_space_overlay};
  math::uuid camera{};      // only meaningful for screen_space_camera/world_space; unused in v1
  std::int32_t sort_order{0};
}; // struct canvas

enum class [[=reflection::named]] canvas_scale_mode : std::uint8_t {
  constant_pixel_size,
  scale_with_screen_size,
  constant_physical_size,
}; // enum class canvas_scale_mode

struct canvas_scaler {
  canvas_scale_mode mode{canvas_scale_mode::constant_pixel_size};
  math::vector2 reference_resolution{1920.0f, 1080.0f};
  std::float_t match_width_or_height{0.0f};
}; // struct canvas_scaler

struct rect_transform {
  math::vector2 anchor_min{0.5f, 0.5f};
  math::vector2 anchor_max{0.5f, 0.5f};
  math::vector2 anchored_position{0.0f, 0.0f};
  math::vector2 size_delta{100.0f, 30.0f};
  math::vector2 pivot{0.5f, 0.5f};
}; // struct rect_transform

struct canvas_group {
  std::float_t alpha{1.0f};
  bool interactable{true};
  bool blocks_raycasts{true};
  bool ignore_parent_groups{false};
}; // struct canvas_group

struct ui_image {
  assets::texture_handle sprite{};
  math::color tint{1.0f, 1.0f, 1.0f, 1.0f};
  math::vector4 uv_rect{0.0f, 0.0f, 1.0f, 1.0f};
  bool raycast_target{true};
}; // struct ui_image

struct ui_text {
  std::string text{};
  assets::font_handle font{};
  std::float_t font_size{16.0f};
  math::color color{1.0f, 1.0f, 1.0f, 1.0f};
  text_align horizontal_align{text_align::start};
  text_align vertical_align{text_align::start};
  std::float_t line_spacing{1.0f};
  bool raycast_target{true};
}; // struct ui_text

struct ui_button {
  bool interactable{true};
  math::color normal_color{0.25f, 0.25f, 0.25f, 1.0f};
  math::color hovered_color{0.35f, 0.35f, 0.35f, 1.0f};
  math::color pressed_color{0.15f, 0.15f, 0.15f, 1.0f};

  bool is_hovered{false};
  bool is_pressed{false};
  bool was_clicked{false};
}; // struct ui_button

struct layout_element {
  std::float_t min_width{-1.0f};
  std::float_t min_height{-1.0f};
  std::float_t preferred_width{-1.0f};
  std::float_t preferred_height{-1.0f};
  std::float_t flexible_width{-1.0f};
  std::float_t flexible_height{-1.0f};
  bool ignore_layout{false};
}; // struct layout_element

enum class [[=reflection::named]] content_fit_mode : std::uint8_t {
  unconstrained,
  min_size,
  preferred_size,
}; // enum class content_fit_mode

struct content_size_fitter {
  content_fit_mode horizontal_fit{content_fit_mode::unconstrained};
  content_fit_mode vertical_fit{content_fit_mode::unconstrained};
}; // struct content_size_fitter

enum class [[=reflection::named]] layout_alignment : std::uint8_t {
  upper_left, upper_center, upper_right,
  middle_left, middle_center, middle_right,
  lower_left, lower_center, lower_right,
}; // enum class layout_alignment

struct horizontal_layout_group {
  std::float_t spacing{0.0f};
  math::vector4 padding{0.0f, 0.0f, 0.0f, 0.0f}; // left, top, right, bottom
  layout_alignment child_alignment{layout_alignment::upper_left};
  bool control_child_width{true};
  bool control_child_height{true};
  bool child_force_expand_width{false};
  bool child_force_expand_height{false};
}; // struct horizontal_layout_group

struct vertical_layout_group {
  std::float_t spacing{0.0f};
  math::vector4 padding{0.0f, 0.0f, 0.0f, 0.0f}; // left, top, right, bottom
  layout_alignment child_alignment{layout_alignment::upper_left};
  bool control_child_width{true};
  bool control_child_height{true};
  bool child_force_expand_width{false};
  bool child_force_expand_height{false};
}; // struct vertical_layout_group

enum class [[=reflection::named]] grid_start_corner : std::uint8_t {
  upper_left,
  upper_right,
  lower_left,
  lower_right,
}; // enum class grid_start_corner

enum class [[=reflection::named]] grid_start_axis : std::uint8_t {
  horizontal,
  vertical,
}; // enum class grid_start_axis

enum class [[=reflection::named]] grid_constraint : std::uint8_t {
  flexible,
  fixed_column_count,
  fixed_row_count,
}; // enum class grid_constraint

struct grid_layout_group {
  math::vector2 cell_size{100.0f, 100.0f};
  math::vector2 spacing{0.0f, 0.0f};
  math::vector4 padding{0.0f, 0.0f, 0.0f, 0.0f}; // left, top, right, bottom
  layout_alignment child_alignment{layout_alignment::upper_left};
  grid_start_corner start_corner{grid_start_corner::upper_left};
  grid_start_axis start_axis{grid_start_axis::horizontal};
  grid_constraint constraint{grid_constraint::flexible};
  std::int32_t constraint_count{1};
}; // struct grid_layout_group

} // namespace sbx::canvas

#endif // LIBSBX_CANVAS_COMPONENTS_HPP_
