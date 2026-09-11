// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <editor/panels/hierarchy_panel.hpp>

#include <cfloat>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <fmt/format.h>

#include <imgui.h>

#include <libsbx/reflection/enum.hpp>

#include <libsbx/render/ui/fonts/material_design_icons.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/scenes/components.hpp>
#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/assets/primitive_meshes.hpp>

#include <editor/commands/component_commands.hpp>
#include <editor/commands/scene_commands.hpp>

namespace editor {

// Payload carries the dragged node's uuid as a raw uint64_t (sbx::math::uuid::value_type).
inline constexpr auto node_drag_drop_payload_type = "HIERARCHY_NODE";

auto icon_for(const sbx::scenes::node& node) -> const char* {
  if (node.has_component<sbx::scenes::camera>()) return ICON_MDI_CAMERA_OUTLINE;
  if (node.has_component<sbx::scenes::directional_light>()) return ICON_MDI_WHITE_BALANCE_SUNNY;
  if (node.has_component<sbx::scenes::point_light>()) return ICON_MDI_LIGHTBULB_OUTLINE;
  if (node.has_component<sbx::scenes::spot_light>()) return ICON_MDI_FLASHLIGHT;
  if (node.has_component<sbx::scenes::skybox>()) return ICON_MDI_EARTH;
  if (node.has_component<sbx::scenes::mesh_renderer>()) return ICON_MDI_CUBE_OUTLINE;
  return ICON_MDI_AXIS_ARROW;
}

auto draw_3d_object_submenu(editor_state& state, sbx::scenes::scene& scene, std::optional<sbx::math::uuid> parent_id) -> void {
  if (!ImGui::BeginMenu(ICON_MDI_AXIS_ARROW " 3D Object")) {
    return;
  }

  for (const auto kind : sbx::reflection::enum_values<sbx::assets::primitive_mesh_kind>()) {
    if (ImGui::MenuItem(std::string{sbx::reflection::to_string(kind)}.c_str())) {
      auto command = std::make_unique<create_primitive_node_command>(kind, parent_id);
      auto* created = command.get();
      state.push_command(std::move(command));
      state.select_node(scene.find(created->id()));
    }
  }

  ImGui::EndMenu();
}

auto hierarchy_panel::_draw_node_row(editor_state& state, sbx::scenes::scene& scene, sbx::ecs::entity entity) -> void {
  auto node = scene.node_of(entity);

  if (!node.is_valid()) {
    return;
  }

  const auto& relationship = node.get_component<sbx::scenes::relationship>();
  const auto& tag = node.name();
  const auto is_renaming = node.id() == _renaming_id;

  auto flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

  if (relationship.children.empty()) {
    flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  if (state.is_node_selected(node)) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

  ImGui::PushID(static_cast<std::int32_t>(entity));

  const auto is_open = is_renaming
    ? ImGui::TreeNodeEx("##node_row", flags, "%s", icon_for(node))
    : ImGui::TreeNodeEx("##node_row", flags, "%s %s", icon_for(node), tag.c_str());

  if (!is_renaming) {
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      state.select_node(node);
    }

    if (ImGui::BeginDragDropSource()) {
      const auto raw_id = node.id().value();
      ImGui::SetDragDropPayload(node_drag_drop_payload_type, &raw_id, sizeof(raw_id));
      ImGui::Text("%s %s", icon_for(node), tag.c_str());
      ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
      if (const auto* payload = ImGui::AcceptDragDropPayload(node_drag_drop_payload_type)) {
        const auto dragged_value = *static_cast<const std::uint64_t*>(payload->Data);
        _try_reparent(scene, sbx::math::uuid::from_value(dragged_value), node.id(), relationship.children.size());
      }

      ImGui::EndDragDropTarget();
    }
  }

  if (is_renaming) {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);

    if (_rename_focus_pending) {
      ImGui::SetKeyboardFocusHere();
      _rename_focus_pending = false;
    }

    const auto submitted = ImGui::InputText("##rename", _rename_buffer.data(), _rename_buffer.size(), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
    const auto deactivated = ImGui::IsItemDeactivated();
    const auto cancelled = deactivated && ImGui::IsKeyPressed(ImGuiKey_Escape, false);

    if (submitted || (deactivated && !cancelled)) {
      _commit_rename(state, node);
    }

    if (submitted || deactivated) {
      _renaming_id = sbx::math::uuid::nil();
    }
  }

  if (ImGui::BeginPopupContextItem("##node_context")) {
    if (ImGui::MenuItem(ICON_MDI_PLUS " Add Child")) {
      _pending_add_child_parent_id = node.id();
    }

    draw_3d_object_submenu(state, scene, node.id());

    if (ImGui::MenuItem(ICON_MDI_PENCIL " Rename")) {
      _begin_rename(node);
    }

    if (ImGui::MenuItem(ICON_MDI_DELETE " Delete Node")) {
      _pending_delete_id = node.id();
    }

    ImGui::EndPopup();
  }

  if (is_open && !relationship.children.empty()) {
    _draw_child_rows(state, scene, node.id(), relationship.children);

    ImGui::TreePop();
  }

  ImGui::PopID();
}

auto hierarchy_panel::_draw_child_rows(editor_state& state, sbx::scenes::scene& scene, sbx::math::uuid parent_id, const std::vector<sbx::ecs::entity>& children) -> void {
  for (auto i = std::size_t{0u}; i < children.size(); ++i) {
    _draw_drop_zone(scene, parent_id, i);
    _draw_node_row(state, scene, children[i]);
  }

  _draw_drop_zone(scene, parent_id, children.size());
}

auto hierarchy_panel::_draw_drop_zone(sbx::scenes::scene& scene, std::optional<sbx::math::uuid> parent_id, std::size_t index) -> void {
  const auto label = fmt::format("##drop_{}_{}", parent_id ? parent_id->value() : std::uint64_t{0u}, index);

  const auto cursor = ImGui::GetCursorScreenPos();
  const auto width = ImGui::GetContentRegionAvail().x;
  constexpr auto height = 6.0f;

  ImGui::InvisibleButton(label.c_str(), ImVec2(width, height));

  if (ImGui::BeginDragDropTarget()) {
    if (const auto* payload = ImGui::AcceptDragDropPayload(node_drag_drop_payload_type)) {
      const auto dragged_value = *static_cast<const std::uint64_t*>(payload->Data);
      _try_reparent(scene, sbx::math::uuid::from_value(dragged_value), parent_id, index);
    }

    if (const auto* preview = ImGui::GetDragDropPayload(); preview != nullptr && preview->IsDataType(node_drag_drop_payload_type)) {
      const auto y = cursor.y + height * 0.5f;
      ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x, y), ImVec2(cursor.x + width, y), IM_COL32(255, 170, 40, 255), 2.0f);
    }

    ImGui::EndDragDropTarget();
  }
}

auto hierarchy_panel::_try_reparent(sbx::scenes::scene& scene, sbx::math::uuid dragged_id, std::optional<sbx::math::uuid> new_parent_id, std::size_t new_index) -> void {
  if (new_parent_id == dragged_id) {
    return; // can't parent a node to itself
  }

  auto dragged = scene.find(dragged_id);

  if (!dragged.is_valid()) {
    return;
  }

  // Walk up from the prospective new parent to the root — if we pass through the dragged node
  // itself, this drop would parent it under one of its own descendants.
  if (new_parent_id) {
    auto ancestor = scene.find(*new_parent_id);

    while (ancestor.is_valid()) {
      if (ancestor.id() == dragged_id) {
        return;
      }

      auto next = scene.node_of(ancestor.get_component<sbx::scenes::relationship>().parent);

      if (!next.has_component<sbx::scenes::id>()) {
        break;
      }

      ancestor = next;
    }
  }

  _pending_reparent = pending_reparent{dragged_id, new_parent_id, new_index};
}

auto hierarchy_panel::_begin_rename(const sbx::scenes::node& node) -> void {
  _renaming_id = node.id();

  const auto& current_name = node.name();
  std::strncpy(_rename_buffer.data(), current_name.c_str(), _rename_buffer.size() - 1u);
  _rename_buffer[_rename_buffer.size() - 1u] = '\0';

  _rename_focus_pending = true;
}

auto hierarchy_panel::_commit_rename(editor_state& state, sbx::scenes::node& node) -> void {
  const auto before = node.name();
  const auto after = sbx::scenes::tag{std::string{_rename_buffer.data()}};

  if (before == after) {
    return;
  }

  // scene::find(name) can go stale after this (scene::_entities_by_name is populated at creation
  // only) — fine, selection/hierarchy key on entity/id, never name.
  node.name() = after;

  state.push_command(std::make_unique<modify_component_command<sbx::scenes::tag>>(node.id(), before, after, "Rename Node"));
}

auto hierarchy_panel::draw(editor_state& state) -> void {
  ImGui::Begin(window_name);

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  auto& scene = scenes_module.active_scene();

  const auto& top_level = scene.root().get_component<sbx::scenes::relationship>().children;

  for (auto i = std::size_t{0u}; i < top_level.size(); ++i) {
    _draw_drop_zone(scene, std::nullopt, i);
    _draw_node_row(state, scene, top_level[i]);
  }

  _draw_drop_zone(scene, std::nullopt, top_level.size());

  if (top_level.empty()) {
    ImGui::TextDisabled("No nodes in the active scene.");
  }

  if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
    state.clear_selection();
  }

  if (ImGui::BeginPopupContextWindow("##hierarchy_context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
    if (ImGui::MenuItem(ICON_MDI_PLUS " Add Node")) {
      auto command = std::make_unique<create_node_command>();
      auto* created = command.get();
      state.push_command(std::move(command));
      state.select_node(scene.find(created->id()));
    }

    draw_3d_object_submenu(state, scene, std::nullopt);

    ImGui::EndPopup();
  }

  if (ImGui::IsWindowHovered() && ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
    if (auto selected = state.selected_node(scene); selected.is_valid()) {
      _pending_delete_id = selected.id();
    }
  }

  if (ImGui::IsWindowHovered() && ImGui::IsKeyPressed(ImGuiKey_F2, false)) {
    if (auto selected = state.selected_node(scene); selected.is_valid()) {
      _begin_rename(selected);
    }
  }

  if (_pending_reparent) {
    if (auto target = scene.find(_pending_reparent->dragged_id); target.is_valid()) {
      state.push_command(std::make_unique<reparent_node_command>(target, _pending_reparent->new_parent_id, _pending_reparent->new_index));
    }

    _pending_reparent.reset();
  }

  if (_pending_delete_id != sbx::math::uuid::nil()) {
    if (auto target = scene.find(_pending_delete_id); target.is_valid()) {
      state.push_command(std::make_unique<delete_node_command>(target));
    }

    _pending_delete_id = sbx::math::uuid::nil();
  }

  if (_pending_add_child_parent_id != sbx::math::uuid::nil()) {
    if (auto parent = scene.find(_pending_add_child_parent_id); parent.is_valid()) {
      auto command = std::make_unique<create_node_command>(parent.id());
      auto* created = command.get();
      state.push_command(std::move(command));
      state.select_node(scene.find(created->id()));
    }

    _pending_add_child_parent_id = sbx::math::uuid::nil();
  }

  ImGui::End();
}

} // namespace editor
