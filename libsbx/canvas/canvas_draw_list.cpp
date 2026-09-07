// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/canvas/canvas_draw_list.hpp>

namespace sbx::canvas {

auto to_ndc(const math::vector2& pixel, const math::vector2& screen_size) -> math::vector4 {
  return math::vector4{
    (pixel.x() / screen_size.x()) * 2.0f - 1.0f,
    (pixel.y() / screen_size.y()) * 2.0f - 1.0f,
    0.0f,
    1.0f
  };
}

auto push_quad(std::vector<canvas_vertex>& vertices, const math::vector2& top_left, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::vector2& screen_size, const math::vector4& clip_rect) -> void {
  const auto top_right = math::vector2{top_left.x() + size.x(), top_left.y()};
  const auto bottom_left = math::vector2{top_left.x(), top_left.y() + size.y()};
  const auto bottom_right = math::vector2{top_left.x() + size.x(), top_left.y() + size.y()};

  const auto ndc_top_left = to_ndc(top_left, screen_size);
  const auto ndc_top_right = to_ndc(top_right, screen_size);
  const auto ndc_bottom_left = to_ndc(bottom_left, screen_size);
  const auto ndc_bottom_right = to_ndc(bottom_right, screen_size);

  const auto uv_top_left = math::vector2{uv_rect.x(), uv_rect.y()};
  const auto uv_top_right = math::vector2{uv_rect.z(), uv_rect.y()};
  const auto uv_bottom_left = math::vector2{uv_rect.x(), uv_rect.w()};
  const auto uv_bottom_right = math::vector2{uv_rect.z(), uv_rect.w()};

  vertices.push_back(canvas_vertex{ndc_top_left, color, uv_top_left, texture_index, 0u, clip_rect});
  vertices.push_back(canvas_vertex{ndc_bottom_left, color, uv_bottom_left, texture_index, 0u, clip_rect});
  vertices.push_back(canvas_vertex{ndc_top_right, color, uv_top_right, texture_index, 0u, clip_rect});

  vertices.push_back(canvas_vertex{ndc_top_right, color, uv_top_right, texture_index, 0u, clip_rect});
  vertices.push_back(canvas_vertex{ndc_bottom_left, color, uv_bottom_left, texture_index, 0u, clip_rect});
  vertices.push_back(canvas_vertex{ndc_bottom_right, color, uv_bottom_right, texture_index, 0u, clip_rect});
}

auto canvas_draw_list::add_quad(const math::vector2& top_left, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::vector2& screen_size, const math::vector4& clip_rect) -> void {
  push_quad(_vertices, top_left, size, uv_rect, texture_index, color, screen_size, clip_rect);
}

auto canvas_draw_list::add_glyph_quad(const math::vector2& top_left, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::vector2& screen_size, const math::vector4& clip_rect) -> void {
  push_quad(_glyph_vertices, top_left, size, uv_rect, texture_index, color, screen_size, clip_rect);
}

auto to_ndc_world(const math::vector2& local_position, const math::matrix4x4& model_view_projection) -> math::vector4 {
  const auto clip = model_view_projection * math::vector4{local_position.x(), local_position.y(), 0.0f, 1.0f};
  const auto w = clip.w() != 0.0f ? clip.w() : 1.0f;

  return math::vector4{clip.x() / w, clip.y() / w, clip.z() / w, 1.0f};
}

auto push_quad_world(std::vector<canvas_vertex>& vertices, const math::vector2& top_left, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::matrix4x4& model_view_projection, const math::vector4& clip_rect) -> void {
  const auto top_right = math::vector2{top_left.x() + size.x(), top_left.y()};
  const auto bottom_left = math::vector2{top_left.x(), top_left.y() + size.y()};
  const auto bottom_right = math::vector2{top_left.x() + size.x(), top_left.y() + size.y()};

  const auto ndc_top_left = to_ndc_world(top_left, model_view_projection);
  const auto ndc_top_right = to_ndc_world(top_right, model_view_projection);
  const auto ndc_bottom_left = to_ndc_world(bottom_left, model_view_projection);
  const auto ndc_bottom_right = to_ndc_world(bottom_right, model_view_projection);

  const auto uv_top_left = math::vector2{uv_rect.x(), uv_rect.y()};
  const auto uv_top_right = math::vector2{uv_rect.z(), uv_rect.y()};
  const auto uv_bottom_left = math::vector2{uv_rect.x(), uv_rect.w()};
  const auto uv_bottom_right = math::vector2{uv_rect.z(), uv_rect.w()};

  vertices.push_back(canvas_vertex{ndc_top_left, color, uv_top_left, texture_index, 0u, clip_rect});
  vertices.push_back(canvas_vertex{ndc_bottom_left, color, uv_bottom_left, texture_index, 0u, clip_rect});
  vertices.push_back(canvas_vertex{ndc_top_right, color, uv_top_right, texture_index, 0u, clip_rect});

  vertices.push_back(canvas_vertex{ndc_top_right, color, uv_top_right, texture_index, 0u, clip_rect});
  vertices.push_back(canvas_vertex{ndc_bottom_left, color, uv_bottom_left, texture_index, 0u, clip_rect});
  vertices.push_back(canvas_vertex{ndc_bottom_right, color, uv_bottom_right, texture_index, 0u, clip_rect});
}

auto canvas_draw_list::add_quad_world(const math::vector2& top_left, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::matrix4x4& model_view_projection, const math::vector4& clip_rect) -> void {
  push_quad_world(_world_vertices, top_left, size, uv_rect, texture_index, color, model_view_projection, clip_rect);
}

auto canvas_draw_list::add_glyph_quad_world(const math::vector2& top_left, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::matrix4x4& model_view_projection, const math::vector4& clip_rect) -> void {
  push_quad_world(_world_glyph_vertices, top_left, size, uv_rect, texture_index, color, model_view_projection, clip_rect);
}

} // namespace sbx::canvas
