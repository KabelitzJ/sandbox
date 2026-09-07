// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/canvas/canvas_module.hpp>

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include <libsbx/platform/input.hpp>
#include <libsbx/platform/mouse_button.hpp>
#include <libsbx/platform/window.hpp>

#include <libsbx/scenes/components.hpp>

#include <libsbx/assets/assets_module.hpp>

namespace sbx::canvas {

auto canvas_module::update() -> void {
  auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
  auto& scene = scenes_module.active_scene();
  auto& platform_module = core::engine::get_module<platform::platform_module>();
  auto& window = platform_module.window();
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  const auto screen_size = math::vector2{static_cast<std::float_t>(window.width()), static_cast<std::float_t>(window.height())};
  const auto mouse_position = platform::input::mouse_position();
  const auto white_texture_index = assets_module.white_texture()->index();

  _draw_list.clear();
  _wants_pointer_capture = false;

  for (auto&& [entity, button] : scene.query<ui_button>().each()) {
    button.was_clicked = false;
  }

  auto canvases = std::vector<std::pair<scenes::node, canvas>>{};

  for (auto&& [entity, root] : scene.query<canvas>().each()) {
    canvases.emplace_back(scene.node_of(entity), root);
  }

  std::sort(canvases.begin(), canvases.end(), [](const auto& lhs, const auto& rhs) { return lhs.second.sort_order < rhs.second.sort_order; });

  const auto root_state = canvas_inherited_state{};

  for (const auto& [node, root] : canvases) {
    if (root.mode != render_mode::screen_space_overlay) {
      continue;
    }

    if (!node.has_component<scenes::relationship>()) {
      continue;
    }

    auto scale_factor = 1.0f;
    auto virtual_size = screen_size;

    if (node.has_component<canvas_scaler>()) {
      const auto& scaler = node.get_component<canvas_scaler>();

      if (scaler.mode == canvas_scale_mode::scale_with_screen_size && scaler.reference_resolution.x() > 0.0f && scaler.reference_resolution.y() > 0.0f) {
        const auto log_width = std::log2(screen_size.x() / scaler.reference_resolution.x());
        const auto log_height = std::log2(screen_size.y() / scaler.reference_resolution.y());
        const auto blended = log_width + (log_height - log_width) * scaler.match_width_or_height;

        scale_factor = std::pow(2.0f, blended);
        virtual_size = scaler.reference_resolution;
      }
    }

    const auto root_rect = resolved_rect{math::vector2{0.0f, 0.0f}, virtual_size};
    const auto virtual_mouse_position = mouse_position / scale_factor;

    for (const auto child_entity : node.get_component<scenes::relationship>().children) {
      _visit(scene, scene.node_of(child_entity), root_rect, root_state, screen_size, virtual_mouse_position, white_texture_index, scale_factor);
    }
  }
}

auto canvas_module::_visit(scenes::scene& scene, scenes::node node, const resolved_rect& parent_rect, const canvas_inherited_state& inherited, const math::vector2& screen_size, const math::vector2& mouse_position, std::uint32_t white_texture_index, std::float_t scale_factor) -> void {
  if (!node.has_component<rect_transform>()) {
    return;
  }

  auto state = inherited;

  if (node.has_component<canvas_group>()) {
    const auto& group = node.get_component<canvas_group>();

    if (group.ignore_parent_groups) {
      state = canvas_inherited_state{group.alpha, group.interactable, group.blocks_raycasts};
    } else {
      state = canvas_inherited_state{inherited.alpha * group.alpha, inherited.interactable && group.interactable, inherited.blocks_raycasts && group.blocks_raycasts};
    }
  }

  const auto rect = resolve_rect(node.get_component<rect_transform>(), parent_rect);
  const auto screen_rect = resolved_rect{rect.position * scale_factor, rect.size * scale_factor};

  const auto is_over =
    mouse_position.x() >= rect.position.x() && mouse_position.x() <= rect.position.x() + rect.size.x() &&
    mouse_position.y() >= rect.position.y() && mouse_position.y() <= rect.position.y() + rect.size.y();

  const auto full_uv_rect = math::vector4{0.0f, 0.0f, 1.0f, 1.0f};

  if (node.has_component<ui_button>()) {
    auto& button = node.get_component<ui_button>();

    if (button.interactable && state.interactable && state.blocks_raycasts) {
      if (is_over) {
        _wants_pointer_capture = true;

        if (platform::input::is_mouse_button_pressed(platform::mouse_button::left)) {
          button.is_pressed = true;
        }
      }

      if (button.is_pressed && platform::input::is_mouse_button_released(platform::mouse_button::left)) {
        button.was_clicked = is_over;
        button.is_pressed = false;

        if (button.was_clicked) {
          _on_button_clicked.emit(node);
        }
      }

      button.is_hovered = is_over;
    }

    auto fill_color = button.is_pressed ? button.pressed_color : (button.is_hovered ? button.hovered_color : button.normal_color);
    fill_color.a() *= state.alpha;

    _draw_list.add_quad(screen_rect.position, screen_rect.size, full_uv_rect, white_texture_index, fill_color, screen_size);
  } else if (node.has_component<ui_image>()) {
    const auto& image = node.get_component<ui_image>();

    if (image.raycast_target && state.blocks_raycasts && is_over) {
      _wants_pointer_capture = true;
    }

    auto tint = image.tint;
    tint.a() *= state.alpha;

    if (image.sprite && image.sprite->is_valid()) {
      _draw_list.add_quad(screen_rect.position, screen_rect.size, image.uv_rect, image.sprite->index(), tint, screen_size);
    } else {
      _draw_list.add_quad(screen_rect.position, screen_rect.size, full_uv_rect, white_texture_index, tint, screen_size);
    }
  } else if (node.has_component<ui_text>()) {
    const auto& text = node.get_component<ui_text>();

    if (text.raycast_target && state.blocks_raycasts && is_over) {
      _wants_pointer_capture = true;
    }

    auto color = text.color;
    color.a() *= state.alpha;

    for (const auto& glyph : shape_text(text, rect)) {
      _draw_list.add_glyph_quad(glyph.rect.position * scale_factor, glyph.rect.size * scale_factor, glyph.uv_rect, glyph.texture_index, color, screen_size);
    }
  }

  if (node.has_component<scenes::relationship>()) {
    for (const auto child_entity : node.get_component<scenes::relationship>().children) {
      _visit(scene, scene.node_of(child_entity), rect, state, screen_size, mouse_position, white_texture_index, scale_factor);
    }
  }
}

} // namespace sbx::canvas
