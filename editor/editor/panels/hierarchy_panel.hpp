// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef EDITOR_PANELS_HIERARCHY_PANEL_HPP_
#define EDITOR_PANELS_HIERARCHY_PANEL_HPP_

#include <array>
#include <optional>
#include <vector>

#include <libsbx/ecs/entity.hpp>

#include <libsbx/math/uuid.hpp>

#include <libsbx/scenes/scene.hpp>

#include <libsbx/render/ui/fonts/material_design_icons.hpp>

#include <editor/panels/editor_panel.hpp>

namespace editor {

/**
 * @brief The "Hierarchy" panel: a tree of the active scene's nodes. Clicking a row selects that
 * node in the shared editor_state; clicking empty space clears the selection. A toolbar button and
 * per-row context menu create/delete nodes.
 */
class hierarchy_panel final : public editor_panel {

public:

  /** @brief The exact string passed to ImGui::Begin() — the window's identity (icon + label + ###id, all significant). Single source of truth: also referenced by editor_ui_layer's default dock layout, so a rename here can't silently desync it. */
  inline static constexpr auto window_name = ICON_MDI_FILE_TREE " Hierarchy###hierarchy_panel";

  auto draw(editor_state& state) -> void override;

private:

  auto _draw_node_row(editor_state& state, sbx::scenes::scene& scene, sbx::ecs::entity entity) -> void;

  /** @brief Draws children as alternating drop-zone/row pairs (n+1 zones for n children), so a drop can land at any index including first/last. */
  auto _draw_child_rows(editor_state& state, sbx::scenes::scene& scene, sbx::math::uuid parent_id, const std::vector<sbx::ecs::entity>& children) -> void;

  /** @brief Thin drop target between two siblings — accepting a payload there inserts at index among parent_id's children (nullopt = top-level). */
  auto _draw_drop_zone(sbx::scenes::scene& scene, std::optional<sbx::math::uuid> parent_id, std::size_t index) -> void;

  /**
   * @brief Validates the drop (no self-parenting, no dropping onto your own descendant) and, if
   * sound, records it into _pending_reparent for draw() to apply once the tree is done drawing.
   *
   * Never mutates the scene graph itself: a drop target is evaluated mid-traversal, and applying
   * scene::insert_child() right there would resize/erase a relationship::children vector that an
   * enclosing _draw_child_rows call still holds a reference into and is mid-iteration over —
   * exactly the kind of container mutation ImGui's tree/ID stack can't survive (surfaces as
   * "Missing TreePop()" / "PopID() called too many times").
   */
  auto _try_reparent(sbx::scenes::scene& scene, sbx::math::uuid dragged_id, std::optional<sbx::math::uuid> new_parent_id, std::size_t new_index) -> void;

  auto _begin_rename(const sbx::scenes::node& node) -> void;

  auto _commit_rename(editor_state& state, sbx::scenes::node& node) -> void;

  sbx::math::uuid _pending_delete_id{sbx::math::uuid::nil()};
  sbx::math::uuid _pending_add_child_parent_id{sbx::math::uuid::nil()};

  /** @brief One drag/drop reparent request, applied after the tree has fully drawn this frame (see _try_reparent). */
  struct pending_reparent {
    sbx::math::uuid dragged_id;
    std::optional<sbx::math::uuid> new_parent_id;
    std::size_t new_index;
  };

  std::optional<pending_reparent> _pending_reparent{};

  sbx::math::uuid _renaming_id{sbx::math::uuid::nil()};
  std::array<char, 256u> _rename_buffer{};
  bool _rename_focus_pending{false};

}; // class hierarchy_panel

} // namespace editor

#endif // EDITOR_PANELS_HIERARCHY_PANEL_HPP_
