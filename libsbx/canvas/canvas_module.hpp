// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_CANVAS_CANVAS_MODULE_HPP_
#define LIBSBX_CANVAS_CANVAS_MODULE_HPP_

#include <string>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/uuid.hpp>

#include <libsbx/platform/platform_module.hpp>

#include <libsbx/assets/assets_module.hpp>
#include <libsbx/assets/font.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/containers/dense_map.hpp>

#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/canvas/components.hpp>
#include <libsbx/canvas/rect_resolve.hpp>
#include <libsbx/canvas/text_layout.hpp>
#include <libsbx/canvas/layout_resolve.hpp>
#include <libsbx/canvas/canvas_draw_list.hpp>

namespace sbx::canvas {

struct canvas_inherited_state {
  std::float_t alpha{1.0f};
  bool interactable{true};
  bool blocks_raycasts{true};
  math::vector4 clip_rect{-1e9f, -1e9f, 1e9f, 1e9f};
}; // struct canvas_inherited_state

/**
 * @brief Every input shape_text's output depends on -- see text_layout.cpp's shape_text -- except
 * ui_text::color (applied separately, when the shaped glyphs are turned into draw quads, not part
 * of the shaping itself). font is stored as a raw pointer (not the assets::font_handle itself,
 * which has no operator== of its own) -- comparing the pointee is exactly "is this the same loaded
 * font asset", which is what a handle change would mean here anyway.
 *
 * font_generation is *not* redundant with the font pointer: a font's identity is assigned
 * synchronously (assets::asset_residency::load_font) but its glyph table arrives later, off the
 * background asset loader -- the pointer alone doesn't change when that happens, so without this
 * field the very first shape (while the font is still an empty placeholder) would get cached
 * forever, permanently rendering no text even once the real glyph data is in. See
 * assets::font::generation's doc comment.
 */
struct text_shape_cache_key {
  std::string text{};
  const assets::font* font{nullptr};
  std::uint64_t font_generation{0u};
  std::float_t font_size{0.0f};
  text_align horizontal_align{};
  text_align vertical_align{};
  std::float_t line_spacing{0.0f};
  resolved_rect rect{};

  auto operator==(const text_shape_cache_key&) const -> bool = default;
}; // struct text_shape_cache_key

struct text_shape_cache_entry {
  text_shape_cache_key key{};
  std::vector<text_glyph> glyphs{};
}; // struct text_shape_cache_entry

class canvas_module final : public utility::noncopyable {

public:

  using dependencies = core::dependency_list<scenes::scenes_module, platform::platform_module, assets::assets_module>;

  auto update() -> void;

  [[nodiscard]] auto wants_pointer_capture() const noexcept -> bool {
    return _wants_pointer_capture;
  }

  [[nodiscard]] auto draw_list() noexcept -> canvas_draw_list& {
    return _draw_list;
  }

  [[nodiscard]] auto on_button_clicked() noexcept -> signals::signal<const scenes::node&>& {
    return _on_button_clicked;
  }

  [[nodiscard]] auto on_value_changed() noexcept -> signals::signal<const scenes::node&>& {
    return _on_value_changed;
  }

private:

  auto _visit(scenes::scene& scene, scenes::node node, const resolved_rect& parent_rect, const canvas_inherited_state& inherited, const math::vector2& screen_size, const math::vector2& mouse_position, std::uint32_t white_texture_index, std::float_t scale_factor, const resolved_rect* rect_override, const math::matrix4x4* world_mvp, bool mask_clip_supported) -> void;

  auto _emit_quad(const math::vector2& position, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::vector2& screen_size, const math::vector4& clip_rect, const math::matrix4x4* world_mvp) -> void;

  auto _emit_glyph_quad(const math::vector2& position, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::vector2& screen_size, const math::vector4& clip_rect, const math::matrix4x4* world_mvp) -> void;

  canvas_draw_list _draw_list{};
  bool _wants_pointer_capture{false};
  signals::signal<const scenes::node&> _on_button_clicked{};
  signals::signal<const scenes::node&> _on_value_changed{};

  // shape_text's output, memoized per ui_text node -- see text_shape_cache_key's doc comment.
  // Recomputed only when this frame's key differs from the one that produced the cached glyphs, so
  // a node whose text/font/size/alignment/rect haven't changed since last frame (the common case for
  // HUD/menu text) skips glyph shaping entirely.
  //
  // Keyed by the node's stable scenes::id (a math::uuid) -- canvas has no business knowing about
  // ecs::entity, and a uuid is a plain value with no live ECS state behind it, so hashing/comparing/
  // erasing one is always safe regardless of whether the node it once named still exists. This also
  // sidesteps what broke the previous scenes::node-keyed version: std::hash<scenes::node> reads the
  // entity's id *component* (see node.hpp), which asserts on a destroyed entity, and dense_map has
  // to hash a key both to insert/rehash *and* to erase it (it re-derives the bucket from the key, it
  // doesn't remember it) -- so a stale scenes::node entry crashed the very prune meant to remove it.
  // update() clears this outright on a scene swap and prunes individually-dead nodes every frame
  // otherwise, via scene::find(uuid) -- see its doc comment there.
  containers::dense_map<math::uuid, text_shape_cache_entry> _text_shape_cache{};

  // Identity only, never dereferenced when stale -- compared against scenes_module::active_scene()
  // each frame to detect a scene swap (see update()'s doc comment on _text_shape_cache).
  const scenes::scene* _last_scene{nullptr};

  // Owns layout_horizontal_children/layout_vertical_children/layout_grid_children's scratch
  // storage -- see layout_resolver's own doc comment.
  layout_resolver _layout_resolver{};

}; // class canvas_module

} // namespace sbx::canvas

#endif // LIBSBX_CANVAS_CANVAS_MODULE_HPP_
