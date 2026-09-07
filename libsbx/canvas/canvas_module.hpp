// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_CANVAS_CANVAS_MODULE_HPP_
#define LIBSBX_CANVAS_CANVAS_MODULE_HPP_

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/vector2.hpp>

#include <libsbx/platform/platform_module.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/signals/signal.hpp>

#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/canvas/components.hpp>
#include <libsbx/canvas/rect_resolve.hpp>
#include <libsbx/canvas/text_layout.hpp>
#include <libsbx/canvas/canvas_draw_list.hpp>

namespace sbx::canvas {

struct canvas_inherited_state {
  std::float_t alpha{1.0f};
  bool interactable{true};
  bool blocks_raycasts{true};
}; // struct canvas_inherited_state

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

private:

  auto _visit(scenes::scene& scene, scenes::node node, const resolved_rect& parent_rect, const canvas_inherited_state& inherited, const math::vector2& screen_size, const math::vector2& mouse_position, std::uint32_t white_texture_index, std::float_t scale_factor) -> void;

  canvas_draw_list _draw_list{};
  bool _wants_pointer_capture{false};
  signals::signal<const scenes::node&> _on_button_clicked{};

}; // class canvas_module

} // namespace sbx::canvas

#endif // LIBSBX_CANVAS_CANVAS_MODULE_HPP_
