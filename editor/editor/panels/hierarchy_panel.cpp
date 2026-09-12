// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <editor/panels/hierarchy_panel.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
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
#include <editor/commands/composite_command.hpp>
#include <editor/commands/scene_commands.hpp>

namespace editor {

inline constexpr auto node_drag_drop_payload_type = "HIERARCHY_NODE";

enum class drop_zone { 
  before, 
  into, 
  after
}; // enum class drop_zone

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

auto hierarchy_panel::_draw_node_row(editor_state& state, sbx::scenes::scene& scene, sbx::ecs::entity entity, std::optional<sbx::math::uuid> parent_id, std::size_t sibling_index) -> void {
  auto node = scene.node_of(entity);

  if (!node.is_valid()) {
    return;
  }

  _visible_row_order.push_back(node.id());

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

  const auto is_open = is_renaming ? ImGui::TreeNodeEx("##node_row", flags, "%s", icon_for(node)) : ImGui::TreeNodeEx("##node_row", flags, "%s %s", icon_for(node), tag.c_str());

  const auto row_min = ImGui::GetItemRectMin();
  const auto row_max = ImGui::GetItemRectMax();

  const auto row_deactivated = ImGui::IsItemDeactivated();

  if (!is_renaming) {
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      if (ImGui::GetIO().KeyShift) {
        _pending_range_select = pending_range_select{state.node_selection_anchor(), node.id()};
      } else if (ImGui::GetIO().KeyCtrl) {
        state.toggle_node_selection(node);
      } else if (state.is_node_selected(node) && state.selected_node_count() > 1u) {
        _deferred_click_id = node.id();
        _deferred_click_became_drag = false;
      } else {
        state.select_node(node);
      }
    }

    if (ImGui::BeginDragDropSource()) {
      if (node.id() == _deferred_click_id) {
        _deferred_click_became_drag = true;
      }

      if (!state.is_node_selected(node)) {
        state.select_node(node);
      }

      const auto raw_id = node.id().value();
      ImGui::SetDragDropPayload(node_drag_drop_payload_type, &raw_id, sizeof(raw_id));
      ImGui::Text("%s %s", icon_for(node), tag.c_str());
      ImGui::EndDragDropSource();
    }

    if (node.id() == _deferred_click_id && row_deactivated) {
      if (!_deferred_click_became_drag) {
        state.select_node(node);
      }

      _deferred_click_id = sbx::math::uuid::nil();
    }

    if (ImGui::BeginDragDropTarget()) {
      const auto row_height = row_max.y - row_min.y;
      const auto relative_y = row_height > 0.0f ? (ImGui::GetMousePos().y - row_min.y) / row_height : 0.5f;

      const auto zone = relative_y < 0.4f ? drop_zone::before : relative_y > 0.6f ? drop_zone::after : drop_zone::into;

      if (const auto* payload = ImGui::AcceptDragDropPayload(node_drag_drop_payload_type, ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
        const auto dragged_id = sbx::math::uuid::from_value(*static_cast<const std::uint64_t*>(payload->Data));

        switch (zone) {
          case drop_zone::before: _try_reparent(state, scene, dragged_id, parent_id, sibling_index); break;
          case drop_zone::after: _try_reparent(state, scene, dragged_id, parent_id, sibling_index + 1u); break;
          case drop_zone::into: _try_reparent(state, scene, dragged_id, node.id(), relationship.children.size()); break;
        }
      }

      if (const auto* preview = ImGui::GetDragDropPayload(); preview != nullptr && preview->IsDataType(node_drag_drop_payload_type)) {
        auto* draw_list = ImGui::GetWindowDrawList();
        const auto color = ImGui::GetColorU32(ImGuiCol_DragDropTarget);

        switch (zone) {
          case drop_zone::before: draw_list->AddLine(ImVec2(row_min.x, row_min.y), ImVec2(row_max.x, row_min.y), color, 2.0f); break;
          case drop_zone::after: draw_list->AddLine(ImVec2(row_min.x, row_max.y), ImVec2(row_max.x, row_max.y), color, 2.0f); break;
          case drop_zone::into: draw_list->AddRect(row_min, row_max, color, 0.0f, 0, 2.0f); break;
        }
      }

      ImGui::EndDragDropTarget();
    }
  }

  if (is_renaming) {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(std::numeric_limits<std::float_t>::lowest());

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

    if (state.selected_node_count() <= 1u && ImGui::MenuItem(ICON_MDI_PENCIL " Rename")) {
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
    _draw_node_row(state, scene, children[i], parent_id, i);
  }
}

auto hierarchy_panel::_try_reparent(editor_state& state, sbx::scenes::scene& scene, sbx::math::uuid payload_id, std::optional<sbx::math::uuid> new_parent_id, std::size_t new_index) -> void {
  const auto& selected = state.selected_node_ids();

  const auto has_selected = (selected.size() > 1u && std::find(selected.begin(), selected.end(), payload_id) != selected.end());

  auto dragged_ids = has_selected ? selected : std::vector<sbx::math::uuid>{payload_id};

  if (new_parent_id) {
    if (std::find(dragged_ids.begin(), dragged_ids.end(), *new_parent_id) != dragged_ids.end()) {
      return;
    }

    auto ancestor = scene.find(*new_parent_id);

    while (ancestor.is_valid()) {
      if (std::find(dragged_ids.begin(), dragged_ids.end(), ancestor.id()) != dragged_ids.end()) {
        return;
      }

      auto next = scene.node_of(ancestor.get_component<sbx::scenes::relationship>().parent);

      if (!next.has_component<sbx::scenes::id>()) {
        break;
      }

      ancestor = next;
    }
  }

  _pending_reparent = pending_reparent{std::move(dragged_ids), new_parent_id, new_index};
}

auto hierarchy_panel::_current_parent_id(sbx::scenes::scene& scene, sbx::math::uuid id) const -> std::optional<sbx::math::uuid> {
  auto node = scene.find(id);

  if (!node.is_valid()) {
    return std::nullopt;
  }

  auto parent = scene.node_of(node.get_component<sbx::scenes::relationship>().parent);

  if (!parent.has_component<sbx::scenes::id>()) {
    return std::nullopt;
  }

  return parent.id();
}

auto hierarchy_panel::_filter_to_selection_roots(sbx::scenes::scene& scene, const std::vector<sbx::math::uuid>& ids) const -> std::vector<sbx::math::uuid> {
  auto roots = std::vector<sbx::math::uuid>{};

  for (const auto id : ids) {
    const auto parent_id = _current_parent_id(scene, id);

    if (!parent_id || std::find(ids.begin(), ids.end(), *parent_id) == ids.end()) {
      roots.push_back(id);
    }
  }

  return roots;
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

  node.name() = after;

  state.push_command(std::make_unique<modify_component_command<sbx::scenes::tag>>(node.id(), before, after, "Rename Node"));
}

auto hierarchy_panel::draw(editor_state& state) -> void {
  ImGui::Begin(window_name);

  _visible_row_order.clear();

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  auto& scene = scenes_module.active_scene();

  const auto& top_level = scene.root().get_component<sbx::scenes::relationship>().children;

  for (auto i = std::size_t{0u}; i < top_level.size(); ++i) {
    _draw_node_row(state, scene, top_level[i], std::nullopt, i);
  }

  if (top_level.empty()) {
    ImGui::TextDisabled("No nodes in the active scene.");
  }

  {
    const auto available = ImGui::GetContentRegionAvail();

    if (available.y > 0.0f) {
      ImGui::InvisibleButton("##root_drop_target", available);

      if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) {
        state.clear_selection();
      }

      if (ImGui::BeginDragDropTarget()) {
        if (const auto* payload = ImGui::AcceptDragDropPayload(node_drag_drop_payload_type, ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
          const auto dragged_id = sbx::math::uuid::from_value(*static_cast<const std::uint64_t*>(payload->Data));
          _try_reparent(state, scene, dragged_id, std::nullopt, top_level.size());
        }

        ImGui::EndDragDropTarget();
      }

      if (ImGui::BeginPopupContextItem("##hierarchy_context_empty")) {
        if (ImGui::MenuItem(ICON_MDI_PLUS " Add Node")) {
          auto command = std::make_unique<create_node_command>();
          auto* created = command.get();

          state.push_command(std::move(command));
          state.select_node(scene.find(created->id()));
        }

        draw_3d_object_submenu(state, scene, std::nullopt);

        ImGui::EndPopup();
      }
    }
  }

  if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) {
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
    auto roots = _filter_to_selection_roots(scene, state.selected_node_ids());

    if (roots.size() == 1u) {
      _pending_delete_id = roots.front();
    } else if (roots.size() > 1u) {
      _pending_delete_ids = std::move(roots);
    }
  }

  if (ImGui::IsWindowHovered() && ImGui::IsKeyPressed(ImGuiKey_F2, false) && state.selected_node_count() == 1u) {
    if (auto selected = state.selected_node(scene); selected.is_valid()) {
      _begin_rename(selected);
    }
  }

  if (_pending_reparent) {
    auto ordered = _filter_to_selection_roots(scene, _pending_reparent->dragged_ids);

    std::stable_sort(ordered.begin(), ordered.end(), [this](sbx::math::uuid a, sbx::math::uuid b) {
      const auto index_of = [this](sbx::math::uuid id) {
        const auto entry = std::find(_visible_row_order.begin(), _visible_row_order.end(), id);
        return entry == _visible_row_order.end() ? _visible_row_order.size() : static_cast<std::size_t>(entry - _visible_row_order.begin());
      };

      return index_of(a) < index_of(b);
    });

    if (ordered.size() == 1u) {
      if (auto target = scene.find(ordered.front()); target.is_valid()) {
        state.push_command(std::make_unique<reparent_node_command>(target, _pending_reparent->new_parent_id, _pending_reparent->new_index));
      }
    } else if (ordered.size() > 1u) {
      auto sub_commands = std::vector<std::unique_ptr<command>>{};
      auto foreign_count = std::size_t{0u};

      for (const auto id : ordered) {
        auto target = scene.find(id);

        if (!target.is_valid()) {
          continue;
        }

        const auto was_already_sibling = _current_parent_id(scene, id) == _pending_reparent->new_parent_id;

        sub_commands.push_back(std::make_unique<reparent_node_command>(target, _pending_reparent->new_parent_id, _pending_reparent->new_index + foreign_count));

        if (!was_already_sibling) {
          foreign_count += 1u;
        }
      }

      if (!sub_commands.empty()) {
        state.push_command(std::make_unique<composite_command>(std::move(sub_commands), fmt::format("Move {} Nodes", sub_commands.size())));
      }
    }

    _pending_reparent.reset();
  }

  if (_pending_range_select) {
    const auto begin_entry = std::find(_visible_row_order.begin(), _visible_row_order.end(), _pending_range_select->anchor_id);
    const auto end_entry = std::find(_visible_row_order.begin(), _visible_row_order.end(), _pending_range_select->clicked_id);

    if (begin_entry == _visible_row_order.end() || end_entry == _visible_row_order.end()) {
      if (auto clicked = scene.find(_pending_range_select->clicked_id); clicked.is_valid()) {
        state.select_node(clicked);
      }
    } else {
      const auto [low, high] = std::minmax(begin_entry, end_entry);
      state.set_node_selection(std::vector<sbx::math::uuid>(low, high + 1));
    }

    _pending_range_select.reset();
  }

  if (_pending_delete_id != sbx::math::uuid::nil()) {
    if (auto target = scene.find(_pending_delete_id); target.is_valid()) {
      state.push_command(std::make_unique<delete_node_command>(target));
    }

    _pending_delete_id = sbx::math::uuid::nil();
  }

  if (!_pending_delete_ids.empty()) {
    auto sub_commands = std::vector<std::unique_ptr<command>>{};

    for (const auto id : _pending_delete_ids) {
      if (auto target = scene.find(id); target.is_valid()) {
        sub_commands.push_back(std::make_unique<delete_node_command>(target));
      }
    }

    if (!sub_commands.empty()) {
      state.push_command(std::make_unique<composite_command>(std::move(sub_commands), fmt::format("Delete {} Nodes", sub_commands.size())));
    }

    _pending_delete_ids.clear();
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
