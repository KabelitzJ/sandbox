// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <editor/panels/asset_browser_panel.hpp>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include <fmt/format.h>

#include <imgui.h>

#include <libsbx/render/ui/fonts/material_design_icons.hpp>
#include <libsbx/render/ui/widgets/asset_tile.hpp>

#include <libsbx/core/engine.hpp>
#include <libsbx/core/project.hpp>

#include <libsbx/assets/assets_module.hpp>
#include <libsbx/assets/material.hpp>
#include <libsbx/assets/particle_effect.hpp>
#include <libsbx/assets/animation_graph.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <editor/commands/scene_commands.hpp>

namespace editor {

// Single source of truth for both classify_extension (below) and importable_extensions, so the
// "Import from Disk..." file dialog's filter (see _draw_create_menu) can never drift from what a
// dropped-in file would actually be classified as.
auto extension_table() -> const std::unordered_map<std::string, asset_kind>& {
  static const auto table = std::unordered_map<std::string, asset_kind>{
    {".png", asset_kind::texture},
    {".jpg", asset_kind::texture},
    {".jpeg", asset_kind::texture},
    {".gltf", asset_kind::mesh},
    {".glb", asset_kind::mesh},
    {".material", asset_kind::material},
    {".hdr", asset_kind::environment_map},
    {".particle_effect", asset_kind::particle_effect},
    {".animation_graph", asset_kind::animation_graph},
    {".ttf", asset_kind::font},
    {".prefab", asset_kind::prefab},
    {".yaml", asset_kind::scene},
    {".cs", asset_kind::script},
  };

  return table;
}

[[nodiscard]] auto is_importable_kind(asset_kind kind) -> bool {
  return kind == asset_kind::texture || kind == asset_kind::mesh ||
         kind == asset_kind::material || kind == asset_kind::environment_map ||
         kind == asset_kind::particle_effect || kind == asset_kind::animation_graph ||
         kind == asset_kind::font;
}

// Case-insensitive alphabetical order, shared by the folder tree and the contents pane so both
// panes read as "sorted" the same way.
[[nodiscard]] auto filename_less(const std::filesystem::path& lhs, const std::filesystem::path& rhs) -> bool {
  const auto to_lower = [](const std::filesystem::path& path) {
    auto name = path.filename().string();
    std::ranges::transform(name, name.begin(), [](unsigned char c) { return std::tolower(c); });
    return name;
  };

  return to_lower(lhs) < to_lower(rhs);
}

auto classify_extension(const std::filesystem::path& extension) -> asset_kind {
  const auto& table = extension_table();
  const auto entry = table.find(extension.string());

  return entry != table.end() ? entry->second : asset_kind::unknown;
}

/** @brief Every extension "Import from Disk..."'s file dialog should offer — everything classify_extension routes through assets_module::import (i.e. every importable kind; .yaml/scene is excluded, same as the per-entry Import path). */
auto importable_extensions() -> std::vector<std::string> {
  auto extensions = std::vector<std::string>{};

  for (const auto& [extension, kind] : extension_table()) {
    if (is_importable_kind(kind)) {
      extensions.push_back(extension);
    }
  }

  return extensions;
}

auto icon_for(const asset_browser_entry& entry) -> const char* {
  if (entry.is_directory) {
    return ICON_MDI_FOLDER;
  }

  switch (entry.kind) {
    case asset_kind::texture: return ICON_MDI_IMAGE;
    case asset_kind::mesh: return ICON_MDI_CUBE_OUTLINE;
    case asset_kind::material: return ICON_MDI_PALETTE_SWATCH;
    case asset_kind::environment_map: return ICON_MDI_EARTH;
    case asset_kind::particle_effect: return ICON_MDI_FIREWORK;
    case asset_kind::animation_graph: return ICON_MDI_STATE_MACHINE;
    case asset_kind::font: return ICON_MDI_FORMAT_FONT;
    case asset_kind::prefab: return ICON_MDI_CUBE_SCAN;
    case asset_kind::scene: return ICON_MDI_FILE_TREE;
    case asset_kind::script: return ICON_MDI_FILE_CODE_OUTLINE;
    case asset_kind::unknown: return ICON_MDI_FILE_OUTLINE;
  }

  return ICON_MDI_FILE_OUTLINE;
}

auto is_entry_selected(const editor_state& state, const std::filesystem::path& path) -> bool {
  const auto* selected = std::get_if<asset_selection>(&state.current_selection);
  return selected != nullptr && selected->path == path;
}

// Which drag_drop_payload_* (asset_tile.hpp) a tile of this kind carries -- nullptr for kinds no
// Inspector picker ever accepts (environment_map, scene, script), which just aren't draggable
// into a picker slot (they can still carry the move payload below).
auto drag_payload_type_for(asset_kind kind) -> const char* {
  switch (kind) {
    case asset_kind::texture: return sbx::render::widgets::drag_drop_payload_texture;
    case asset_kind::mesh: return sbx::render::widgets::drag_drop_payload_mesh;
    case asset_kind::material: return sbx::render::widgets::drag_drop_payload_material;
    case asset_kind::particle_effect: return sbx::render::widgets::drag_drop_payload_particle_effect;
    case asset_kind::animation_graph: return sbx::render::widgets::drag_drop_payload_animation_graph;
    case asset_kind::font: return sbx::render::widgets::drag_drop_payload_font;
    case asset_kind::prefab: return sbx::render::widgets::drag_drop_payload_prefab;
    default: return nullptr;
  }
}

// Browser-local "move this into a folder" drag payload -- distinct from the kind-specific
// drag_drop_payload_* above (which Inspector pickers accept), this one only ever moves between
// tiles/tree nodes of this same panel, and (unlike the kind-specific payload) both files and
// folders offer it.
inline constexpr auto asset_move_drag_payload_type = "SBX_ASSET_BROWSER_MOVE";

struct asset_move_drag_payload {
  char path[256]{}; // project-relative, null-terminated
}; // struct asset_move_drag_payload

auto make_move_drag_payload(const std::filesystem::path& relative_path) -> asset_move_drag_payload {
  auto payload = asset_move_drag_payload{};

  const auto path_string = relative_path.string();
  const auto copy_length = std::min(path_string.size(), sizeof(payload.path) - 1u);
  std::memcpy(payload.path, path_string.data(), copy_length);
  payload.path[copy_length] = '\0';

  return payload;
}

auto path_from_move_payload(const ImGuiPayload& payload) -> std::filesystem::path {
  return std::filesystem::path{static_cast<const asset_move_drag_payload*>(payload.Data)->path};
}

// Truncates (with an ellipsis) rather than wrapping, so a long filename never grows a grid row
// taller than the fixed height ImGuiListClipper below assumes, and never overflows sideways into
// the next tile's column.
auto truncate_to_width(const std::string& text, std::float_t max_width) -> std::string {
  if (ImGui::CalcTextSize(text.c_str()).x <= max_width) {
    return text;
  }

  static constexpr auto ellipsis = std::string_view{"..."};

  auto truncated = text;

  while (!truncated.empty() && ImGui::CalcTextSize((truncated + std::string{ellipsis}).c_str()).x > max_width) {
    truncated.pop_back();
  }

  return truncated + std::string{ellipsis};
}

// Internal/tooling entries the Asset Browser should never surface, regardless of the search
// filter: .meta sidecars (asset_cooker's per-source uuid, not content of their own), the IDE-only
// Game.csproj the engine regenerates every start (see script_compiler::_write_ide_project) purely
// for IntelliSense, its bin/obj build output, and any dotfile/hidden entry (.git, .vs, .vscode, ...).
auto is_hidden_from_browser(const std::filesystem::path& name, bool is_directory) -> bool {
  const auto name_string = name.string();
  const auto name_extension = name.extension();

  if (!name_string.empty() && name_string.front() == '.') {
    return true;
  }

  if (is_directory) {
    return name_string == "bin" || name_string == "obj";
  }

  return name_extension == ".meta" || name_extension == ".csproj";
}

// Whether `directory` contains at least one subdirectory the tree would actually show -- used to
// suppress the expand arrow on leaf directories, since there's nothing for it to expand into.
[[nodiscard]] auto has_visible_subdirectories(const std::filesystem::path& directory) -> bool {
  auto ec = std::error_code{};

  for (const auto& entry : std::filesystem::directory_iterator{directory, ec}) {
    if (entry.is_directory(ec) && !is_hidden_from_browser(entry.path().filename(), true)) {
      return true;
    }
  }

  return false;
}

// True if `candidate` is `target` itself or one of its ancestors (path-component prefix) -- used
// to decide which nodes along a "reveal" target's chain need to be forced open, and to refuse a
// drop that would move a folder onto itself or into its own descendant.
[[nodiscard]] auto is_ancestor_or_self(const std::filesystem::path& candidate, const std::filesystem::path& target) -> bool {
  auto candidate_it = candidate.begin();
  auto target_it = target.begin();

  for (; candidate_it != candidate.end(); ++candidate_it, ++target_it) {
    if (target_it == target.end() || *candidate_it != *target_it) {
      return false;
    }
  }

  return true;
}

// Case-insensitive substring test for the search box -- an empty filter matches everything.
auto filter_matches(std::string_view name, std::string_view filter) -> bool {
  if (filter.empty()) {
    return true;
  }

  const auto to_lower = [](std::string_view text) {
    auto result = std::string{text};
    std::ranges::transform(result, result.begin(), [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
    return result;
  };

  return to_lower(name).find(to_lower(filter)) != std::string::npos;
}

auto asset_browser_panel::_refresh_entries() -> void {
  _cached_entries.clear();

  auto& project = sbx::core::engine::project();
  const auto absolute_directory = project.assets_directory() / _current_directory;

  if (!std::filesystem::exists(absolute_directory)) {
    _needs_refresh = false;
    return;
  }

  for (const auto& dir_entry : std::filesystem::directory_iterator{absolute_directory}) {
    if (is_hidden_from_browser(dir_entry.path().filename(), dir_entry.is_directory())) {
      continue;
    }

    auto entry = asset_browser_entry{};
    entry.path = _current_directory / dir_entry.path().filename();
    entry.is_directory = dir_entry.is_directory();

    if (!entry.is_directory) {
      entry.kind = classify_extension(dir_entry.path().extension());
      entry.is_importable = is_importable_kind(entry.kind);
    }

    _cached_entries.push_back(std::move(entry));
  }

  std::ranges::sort(_cached_entries, [](const auto& lhs, const auto& rhs) {
    if (lhs.is_directory != rhs.is_directory) {
      return lhs.is_directory > rhs.is_directory;
    }

    return filename_less(lhs.path, rhs.path);
  });

  _needs_refresh = false;
}

auto asset_browser_panel::_unique_name(const std::filesystem::path& absolute_directory, std::string_view stem, std::string_view extension) const -> std::string {
  auto name = extension.empty() ? std::string{stem} : fmt::format("{}{}", stem, extension);

  if (!std::filesystem::exists(absolute_directory / name)) {
    return name;
  }

  for (auto suffix = 1;; ++suffix) {
    name = extension.empty() ? fmt::format("{} {}", stem, suffix) : fmt::format("{} {}{}", stem, suffix, extension);

    if (!std::filesystem::exists(absolute_directory / name)) {
      return name;
    }
  }
}

auto asset_browser_panel::_create_material(editor_state& state, const std::filesystem::path& target_directory) -> void {
  auto& project = sbx::core::engine::project();
  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  const auto absolute_directory = project.assets_directory() / target_directory;
  std::filesystem::create_directories(absolute_directory);

  const auto relative_path = target_directory / _unique_name(absolute_directory, "New Material", ".material");

  auto handle = assets_module.create_material(sbx::assets::material::create_info{.name = "New Material"});
  const auto id = assets_module.save_material(handle, relative_path);

  _navigate_to(target_directory);
  state.select_asset(id, relative_path, asset_kind::material);
}

auto asset_browser_panel::_create_particle_effect(editor_state& state, const std::filesystem::path& target_directory) -> void {
  auto& project = sbx::core::engine::project();
  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  const auto absolute_directory = project.assets_directory() / target_directory;
  std::filesystem::create_directories(absolute_directory);

  const auto relative_path = target_directory / _unique_name(absolute_directory, "New Particle Effect", ".particle_effect");

  auto handle = assets_module.create_particle_effect(sbx::assets::particle_effect::create_info{.name = "New Particle Effect"});
  const auto id = assets_module.save_particle_effect(handle, relative_path);

  _navigate_to(target_directory);
  state.select_asset(id, relative_path, asset_kind::particle_effect);
}

auto asset_browser_panel::_create_animation_graph(editor_state& state, const std::filesystem::path& target_directory) -> void {
  auto& project = sbx::core::engine::project();
  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  const auto absolute_directory = project.assets_directory() / target_directory;
  std::filesystem::create_directories(absolute_directory);

  const auto relative_path = target_directory / _unique_name(absolute_directory, "New Animation Graph", ".animation_graph");

  // A single entry state so the graph is already is_valid() -- states/transitions beyond this
  // are hand-authored in the saved .animation_graph file until the visual graph editor lands
  // (see the animator's Inspector section, which only edits parameters, not graph structure).
  auto create_info = sbx::assets::animation_graph::create_info{.name = "New Animation Graph"};
  create_info.states.push_back(sbx::assets::animation_state{.id = 0u, .name = "Idle"});
  create_info.entry_state_id = 0u;

  auto handle = assets_module.create_animation_graph(create_info);
  const auto id = assets_module.save_animation_graph(handle, relative_path);

  _navigate_to(target_directory);
  state.select_asset(id, relative_path, asset_kind::animation_graph);
}

auto asset_browser_panel::_create_script(editor_state& state, const std::filesystem::path& target_directory) -> void {
  auto& project = sbx::core::engine::project();

  const auto absolute_directory = project.assets_directory() / target_directory;
  std::filesystem::create_directories(absolute_directory);

  const auto file_name = _unique_name(absolute_directory, "NewScript", ".cs");
  const auto relative_path = target_directory / file_name;
  const auto class_name = std::filesystem::path{file_name}.stem().string();

  auto out = std::ofstream{absolute_directory / file_name};
  out << fmt::format(
    "using Sbx.Core;\n\n"
    "public class {} : Behavior\n"
    "{{\n"
    "    public override void OnCreate()\n"
    "    {{\n"
    "    }}\n\n"
    "    public override void OnUpdate()\n"
    "    {{\n"
    "    }}\n\n"
    "    public override void OnDestroy()\n"
    "    {{\n"
    "    }}\n"
    "}}\n",
    class_name
  );

  _navigate_to(target_directory);
  state.select_asset(sbx::math::uuid::nil(), relative_path, asset_kind::script);
}

auto asset_browser_panel::_create_folder(const std::filesystem::path& target_directory) -> void {
  auto& project = sbx::core::engine::project();

  const auto absolute_directory = project.assets_directory() / target_directory;
  std::filesystem::create_directories(absolute_directory);

  const auto relative_path = target_directory / _unique_name(absolute_directory, "New Folder", "");
  std::filesystem::create_directory(project.assets_directory() / relative_path);

  _navigate_to(target_directory);
  _begin_rename(relative_path, true); // Explorer/Unity both drop straight into rename on create
}

auto asset_browser_panel::_draw_create_menu(editor_state& state, const std::filesystem::path& target_directory) -> void {
  if (ImGui::MenuItem(ICON_MDI_PALETTE_SWATCH " Material")) {
    _create_material(state, target_directory);
  }

  if (ImGui::MenuItem(ICON_MDI_FIREWORK " Particle Effect")) {
    _create_particle_effect(state, target_directory);
  }

  if (ImGui::MenuItem(ICON_MDI_STATE_MACHINE " Animation Graph")) {
    _create_animation_graph(state, target_directory);
  }

  if (ImGui::MenuItem(ICON_MDI_FILE_CODE_OUTLINE " C# Script")) {
    _create_script(state, target_directory);
  }

  ImGui::Separator();

  if (ImGui::MenuItem(ICON_MDI_FOLDER_PLUS " New Folder")) {
    _create_folder(target_directory);
  }

  ImGui::Separator();

  if (ImGui::MenuItem(ICON_MDI_FILE_IMPORT " Import from Disk...")) {
    auto& project = sbx::core::engine::project();
    _import_destination_directory = target_directory;
    _import_dialog.open("Import Asset", sbx::render::widgets::file_dialog_mode::open_files, project.assets_directory() / target_directory, importable_extensions());
  }

  if (ImGui::MenuItem(ICON_MDI_REFRESH " Reimport All in This Folder")) {
    auto& project = sbx::core::engine::project();
    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

    // import_directory (like import()) needs a path resolvable from cwd, not one merely
    // relative to assets_directory() — see assets_module.hpp's doc comment.
    assets_module.import_directory(project.assets_directory() / target_directory);
    _needs_refresh = true;
  }
}

auto asset_browser_panel::_begin_rename(const std::filesystem::path& relative_path, bool is_directory) -> void {
  _renaming_path = relative_path;
  _renaming_is_directory = is_directory;

  const auto name = is_directory ? relative_path.filename().string() : relative_path.stem().string();
  std::strncpy(_rename_buffer.data(), name.c_str(), _rename_buffer.size() - 1u);
  _rename_buffer[_rename_buffer.size() - 1u] = '\0';

  _rename_focus_pending = true;
}

auto asset_browser_panel::_draw_rename_field(editor_state& state, std::float_t width) -> void {
  ImGui::SetNextItemWidth(width);

  if (_rename_focus_pending) {
    ImGui::SetKeyboardFocusHere();
    _rename_focus_pending = false;
  }

  const auto submitted = ImGui::InputText("##rename", _rename_buffer.data(), _rename_buffer.size(), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
  const auto deactivated = ImGui::IsItemDeactivated();
  const auto cancelled = deactivated && ImGui::IsKeyPressed(ImGuiKey_Escape, false);

  if (submitted || (deactivated && !cancelled)) {
    _commit_rename(state);
  }

  if (submitted || deactivated) {
    _renaming_path.clear();
  }
}

auto asset_browser_panel::_commit_rename(editor_state& state) -> void {
  auto& project = sbx::core::engine::project();
  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  const auto typed = std::string{_rename_buffer.data()};

  if (typed.empty()) {
    return;
  }

  // A file keeps its own extension -- a .material can't become something else by rename -- only
  // the stem the user actually edited (see _begin_rename) is replaced. A folder's name is edited
  // in full, there being no extension concept for it.
  const auto new_name = _renaming_is_directory ? typed : typed + _renaming_path.extension().string();
  const auto new_relative = _renaming_path.parent_path() / new_name;

  if (new_relative == _renaming_path) {
    return; // unchanged
  }

  const auto old_absolute = project.assets_directory() / _renaming_path;
  const auto new_absolute = project.assets_directory() / new_relative;

  if (std::filesystem::exists(new_absolute)) {
    return; // name clash in this folder -- silently discard, same as an ordinary click-away cancel
  }

  if (!assets_module.move_asset(old_absolute, new_absolute)) {
    return;
  }

  _needs_refresh = true;

  if (const auto* selected = std::get_if<asset_selection>(&state.current_selection); selected != nullptr && selected->path == _renaming_path) {
    state.select_asset(selected->id, new_relative, selected->kind);
  }
}

auto asset_browser_panel::_duplicate(editor_state& state, const std::filesystem::path& relative_path, bool is_directory) -> void {
  auto& project = sbx::core::engine::project();
  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  const auto absolute_directory = project.assets_directory() / relative_path.parent_path();
  const auto stem = is_directory ? relative_path.filename().string() : relative_path.stem().string();
  const auto extension = is_directory ? std::string{} : relative_path.extension().string();

  const auto new_name = _unique_name(absolute_directory, stem + " Copy", extension);
  const auto source_absolute = project.assets_directory() / relative_path;
  const auto destination_absolute = absolute_directory / new_name;

  auto ec = std::error_code{};

  if (is_directory) {
    // .meta files inside are intentionally copied too and then stripped below, so the duplicate's
    // assets mint fresh uuids on next import instead of sharing identity with the originals.
    std::filesystem::copy(source_absolute, destination_absolute, std::filesystem::copy_options::recursive, ec);

    if (!ec) {
      for (const auto& sub_entry : std::filesystem::recursive_directory_iterator{destination_absolute}) {
        if (sub_entry.path().extension() == ".meta") {
          std::filesystem::remove(sub_entry.path());
        }
      }
    }
  } else {
    std::filesystem::copy_file(source_absolute, destination_absolute, ec);
  }

  if (ec) {
    return;
  }

  _needs_refresh = true;

  const auto new_relative = relative_path.parent_path() / new_name;

  if (!is_directory) {
    if (const auto kind = classify_extension(destination_absolute.extension()); is_importable_kind(kind)) {
      const auto id = assets_module.import(destination_absolute);
      state.select_asset(id, new_relative, kind);
    }
  }
}

auto asset_browser_panel::_request_delete(const std::filesystem::path& relative_path, bool is_directory) -> void {
  _pending_delete_path = relative_path;
  _pending_delete_is_directory = is_directory;
  _show_delete_confirm_dialog = true;
}

auto asset_browser_panel::_try_move(editor_state& state, const std::filesystem::path& source_relative, const std::filesystem::path& destination_directory_relative) -> void {
  if (source_relative.empty() || source_relative.parent_path() == destination_directory_relative) {
    return; // already there
  }

  // Refuses moving a folder onto itself or into one of its own descendants (is_ancestor_or_self
  // with candidate=source catches both: destination == source, and destination under source/).
  if (is_ancestor_or_self(source_relative, destination_directory_relative)) {
    return;
  }

  auto& project = sbx::core::engine::project();
  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  const auto source_absolute = project.assets_directory() / source_relative;
  const auto new_relative = destination_directory_relative / source_relative.filename();
  const auto destination_absolute = project.assets_directory() / new_relative;

  if (std::filesystem::exists(destination_absolute)) {
    return; // name clash in the target folder -- silently refuse, same spirit as rename
  }

  if (!assets_module.move_asset(source_absolute, destination_absolute)) {
    return;
  }

  _needs_refresh = true;

  if (const auto* selected = std::get_if<asset_selection>(&state.current_selection); selected != nullptr && selected->path == source_relative) {
    state.select_asset(selected->id, new_relative, selected->kind);
  }
}

// Recursively lists subdirectories only, live per expanded node — cheap (names only, no imports).
auto asset_browser_panel::_draw_directory_tree(editor_state& state, const std::filesystem::path& absolute_assets_root, const std::filesystem::path& relative_directory) -> void {
  const auto absolute_directory = absolute_assets_root / relative_directory;

  if (!std::filesystem::exists(absolute_directory)) {
    return;
  }

  auto subdirectories = std::vector<std::filesystem::path>{};

  for (const auto& dir_entry : std::filesystem::directory_iterator{absolute_directory}) {
    if (dir_entry.is_directory() && !is_hidden_from_browser(dir_entry.path().filename(), true)) {
      subdirectories.push_back(dir_entry.path());
    }
  }

  std::ranges::sort(subdirectories, filename_less);

  for (const auto& subdirectory : subdirectories) {
    const auto relative_child = relative_directory / subdirectory.filename();
    const auto child_name = subdirectory.filename().string();
    const auto is_leaf = !has_visible_subdirectories(subdirectory);
    const auto is_renaming_this = relative_child == _renaming_path;

    auto flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (_current_directory == relative_child) {
      flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (is_leaf) {
      flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    ImGui::PushID(relative_child.string().c_str());

    if (_pending_reveal && is_ancestor_or_self(relative_child, *_pending_reveal)) {
      ImGui::SetNextItemOpen(true);
    }

    const auto is_open = is_renaming_this
      ? ImGui::TreeNodeEx("##dir", flags, "%s", ICON_MDI_FOLDER)
      : ImGui::TreeNodeEx("##dir", flags, "%s %s", ICON_MDI_FOLDER, child_name.c_str());

    if (_pending_reveal && relative_child == *_pending_reveal) {
      ImGui::SetScrollHereY(0.5f);
    }

    if (!is_renaming_this) {
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        _navigate_to(relative_child);
      }

      const auto move_payload = make_move_drag_payload(relative_child);

      if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload(asset_move_drag_payload_type, &move_payload, sizeof(move_payload));
        ImGui::TextUnformatted(child_name.c_str());
        ImGui::EndDragDropSource();
      }
    }

    if (ImGui::BeginDragDropTarget()) {
      if (const auto* payload = ImGui::AcceptDragDropPayload(asset_move_drag_payload_type)) {
        _try_move(state, path_from_move_payload(*payload), relative_child);
      }

      ImGui::EndDragDropTarget();
    }

    if (is_renaming_this) {
      ImGui::SameLine();
      _draw_rename_field(state, std::numeric_limits<std::float_t>::lowest());
    }

    if (ImGui::BeginPopupContextItem("##dir_context")) {
      if (ImGui::BeginMenu(ICON_MDI_FOLDER_PLUS " Create")) {
        _draw_create_menu(state, relative_child);
        ImGui::EndMenu();
      }

      ImGui::Separator();

      if (ImGui::MenuItem(ICON_MDI_PENCIL " Rename")) {
        _begin_rename(relative_child, true);
      }

      if (ImGui::MenuItem(ICON_MDI_CONTENT_DUPLICATE " Duplicate")) {
        _duplicate(state, relative_child, true);
      }

      if (ImGui::MenuItem(ICON_MDI_DELETE " Delete")) {
        _request_delete(relative_child, true);
      }

      ImGui::EndPopup();
    }

    if (is_open && !is_leaf) {
      _draw_directory_tree(state, absolute_assets_root, relative_child);
      ImGui::TreePop();
    }

    ImGui::PopID();
  }
}

auto asset_browser_panel::draw(editor_state& state) -> void {
  // The two panes below scroll on their own (see panes_height) — the panel itself never needs to.
  ImGui::Begin(window_name, nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

  auto& project = sbx::core::engine::project();
  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  // "Show in Browser" (Inspector's picker slots / asset properties view) -- one-shot, unlike
  // editor_state::current_selection, so it never fights the user for navigating this panel on
  // their own afterward.
  if (state.reveal_in_browser_request) {
    _navigate_to(state.reveal_in_browser_request->parent_path());
    state.reveal_in_browser_request.reset();
  }

  if (auto picked = _import_dialog.result()) {
    _pending_asset_imports.insert(_pending_asset_imports.end(), picked->begin(), picked->end());
  }

  _process_pending_asset_imports(state);

  if (ImGui::Button(ICON_MDI_FOLDER_PLUS " Create")) {
    ImGui::OpenPopup("##asset_browser_create_menu");
  }

  if (ImGui::BeginPopup("##asset_browser_create_menu")) {
    _draw_create_menu(state, _current_directory);
    ImGui::EndPopup();
  }

  ImGui::Separator();

  // Breadcrumb bar: "assets" root button, then one clickable button per path segment. Iterates a
  // snapshot of the path rather than _current_directory itself, since a segment's own click below
  // reassigns _current_directory mid-loop (via _navigate_to), which would otherwise invalidate
  // this loop's iterators. Each button also doubles as a move drop target, so dragging a tile onto
  // an ancestor breadcrumb moves it there.
  if (ImGui::Button(ICON_MDI_FOLDER_OPEN " assets")) {
    _navigate_to(std::filesystem::path{});
  }

  if (ImGui::BeginDragDropTarget()) {
    if (const auto* payload = ImGui::AcceptDragDropPayload(asset_move_drag_payload_type)) {
      _try_move(state, path_from_move_payload(*payload), std::filesystem::path{});
    }

    ImGui::EndDragDropTarget();
  }

  const auto breadcrumb_directory = _current_directory;
  auto breadcrumb_path = std::filesystem::path{};

  for (const auto& segment : breadcrumb_directory) {
    breadcrumb_path /= segment;

    ImGui::SameLine(0.0f, 4.0f);
    ImGui::TextDisabled("%s", ICON_MDI_CHEVRON_RIGHT);
    ImGui::SameLine(0.0f, 4.0f);

    ImGui::PushID(breadcrumb_path.string().c_str());

    if (ImGui::Button(segment.string().c_str())) {
      _navigate_to(breadcrumb_path);
    }

    if (ImGui::BeginDragDropTarget()) {
      if (const auto* payload = ImGui::AcceptDragDropPayload(asset_move_drag_payload_type)) {
        _try_move(state, path_from_move_payload(*payload), breadcrumb_path);
      }

      ImGui::EndDragDropTarget();
    }

    ImGui::PopID();
  }

  ImGui::SameLine();
  ImGui::SetNextItemWidth(160.0f);
  ImGui::InputTextWithHint("##asset_browser_search", ICON_MDI_MAGNIFY " Search...", _search_filter.data(), _search_filter.size());

  ImGui::SameLine();
  ImGui::SetNextItemWidth(120.0f);
  ImGui::SliderFloat("##asset_browser_tile_size", &_tile_size, 48.0f, 128.0f, "%.0f px");

  if (_needs_refresh) {
    _refresh_entries();
  }

  // Space left below the toolbar, minus the table's own per-cell padding (added around each
  // child below on top of whatever height we give it) — both panes get exactly this height, so
  // the table's one row never grows past what's actually left and the panel never overflows.
  const auto panes_height = ImGui::GetContentRegionAvail().y - ImGui::GetStyle().CellPadding.y * 2.0f;

  // Folder tree gets a narrow fixed-width column (user-resizable); contents gets the rest.
  if (ImGui::BeginTable("asset_browser_columns", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableSetupColumn("Folders", ImGuiTableColumnFlags_WidthFixed, 180.0f);
    ImGui::TableSetupColumn("Contents", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::BeginChild("##asset_browser_tree_scroll", ImVec2(0.0f, panes_height));

    const auto root_is_leaf = !has_visible_subdirectories(project.assets_directory());

    auto root_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (_current_directory.empty()) {
      root_flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (root_is_leaf) {
      root_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // Any reveal target lives somewhere under the root by definition.
    if (_pending_reveal) {
      ImGui::SetNextItemOpen(true);
    }

    const auto is_root_open = ImGui::TreeNodeEx("##assets_root", root_flags, "%s assets", ICON_MDI_FOLDER_OPEN);

    if (_pending_reveal && _pending_reveal->empty()) {
      ImGui::SetScrollHereY(0.5f);
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      _navigate_to(std::filesystem::path{});
    }

    if (ImGui::BeginDragDropTarget()) {
      if (const auto* payload = ImGui::AcceptDragDropPayload(asset_move_drag_payload_type)) {
        _try_move(state, path_from_move_payload(*payload), std::filesystem::path{});
      }

      ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginPopupContextItem("##assets_root_context")) {
      if (ImGui::BeginMenu(ICON_MDI_FOLDER_PLUS " Create")) {
        _draw_create_menu(state, std::filesystem::path{});
        ImGui::EndMenu();
      }

      ImGui::EndPopup();
    }

    if (is_root_open && !root_is_leaf) {
      _draw_directory_tree(state, project.assets_directory(), std::filesystem::path{});
      ImGui::TreePop();
    }

    // Consumed for exactly the one frame the reveal target's chain needed forcing open -- lets
    // the tree be collapsed by hand afterward instead of snapping back open every frame.
    _pending_reveal.reset();

    ImGui::EndChild();

    ImGui::TableSetColumnIndex(1);
    ImGui::BeginChild("##asset_browser_contents_scroll", ImVec2(0.0f, panes_height));

    // Entries matching the search box, keeping _cached_entries' existing order (directories
    // first, then case-insensitive alphabetical).
    auto visible = std::vector<std::size_t>{};

    for (auto index = std::size_t{0u}; index < _cached_entries.size(); ++index) {
      if (filter_matches(_cached_entries[index].path.filename().string(), _search_filter.data())) {
        visible.push_back(index);
      }
    }

    if (visible.empty()) {
      ImGui::TextDisabled("Nothing here.");
    }

    const auto avail_width = ImGui::GetContentRegionAvail().x;
    const auto cell_width = _tile_size + 8.0f;
    const auto columns = std::max(std::int32_t{1}, static_cast<std::int32_t>(avail_width / cell_width));

    const auto row_height = _tile_size + ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y;
    const auto row_count = (static_cast<std::int32_t>(visible.size()) + columns - 1) / columns;

    // Clipped by grid row so a folder full of textures only ever loads/thumbnails the tiles
    // actually on screen, instead of every texture in it every frame the panel is open.
    auto clipper = ImGuiListClipper{};
    clipper.Begin(row_count, row_height);

    while (clipper.Step()) {
      for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
        for (auto column = std::int32_t{0}; column < columns; ++column) {
          const auto visible_index = static_cast<std::size_t>(row) * static_cast<std::size_t>(columns) + static_cast<std::size_t>(column);

          if (visible_index >= visible.size()) {
            break;
          }

          if (column > 0) {
            ImGui::SameLine();
          }

          auto& entry = _cached_entries[visible[visible_index]];
          const auto is_renaming_this = entry.path == _renaming_path;

          ImGui::PushID(entry.path.string().c_str());
          ImGui::BeginGroup();

          // Every other kind resolves entry.id lazily, on click (see asset_browser_entry's doc
          // comment) — deliberately, since an importable kind may need a dialog first (mesh's
          // "extract materials?" choice). prefab has no such dialog, so resolve it here instead:
          // a drag started on a tile that's never been clicked ends on release, over a *different*
          // widget (the drop target) — draw_asset_tile's own click detection (InvisibleButton's
          // release-based "pressed") never fires on this tile during that gesture, so a
          // click-resolved id would still be nil for the entire drag, and the payload it carries
          // would be too.
          if (entry.kind == asset_kind::prefab && entry.id == sbx::math::uuid::nil()) {
            entry.id = assets_module.import(project.assets_directory() / entry.path);
          }

          auto tile_desc = sbx::render::widgets::asset_tile_desc{};
          tile_desc.icon_glyph = icon_for(entry);
          tile_desc.is_directory = entry.is_directory;
          tile_desc.is_selected = is_entry_selected(state, entry.path);
          tile_desc.size = ImVec2{_tile_size, _tile_size};
          tile_desc.display_name = entry.path.filename().string();
          tile_desc.drag_payload_type = drag_payload_type_for(entry.kind);
          tile_desc.drag_id = entry.id;
          tile_desc.drag_path = entry.path;

          const auto move_payload = make_move_drag_payload(entry.path);
          tile_desc.secondary_drag_payload_type = asset_move_drag_payload_type;
          tile_desc.secondary_drag_payload_data = &move_payload;
          tile_desc.secondary_drag_payload_size = sizeof(move_payload);

          if (entry.kind == asset_kind::texture) {
            tile_desc.is_texture_thumbnail = true;
            tile_desc.texture = assets_module.load_texture(entry.path);
          }

          const auto tile_result = sbx::render::widgets::draw_asset_tile("##tile", tile_desc);

          if (tile_result.hovered) {
            ImGui::SetTooltip("%s", entry.path.string().c_str());
          }

          if (!is_renaming_this && tile_result.clicked) {
            if (entry.is_directory) {
              _navigate_to(entry.path);
            } else if (entry.is_importable) {
              const auto meta_path = std::filesystem::path{project.assets_directory() / entry.path}.concat(".meta");

              if (entry.kind == asset_kind::mesh && !std::filesystem::exists(meta_path)) {
                // First time this mesh has ever been seen — let the user decide whether to extract
                // its materials before it's actually cooked (deferring would mean the choice has
                // nowhere to be remembered until something else needs the mesh).
                _pending_import_path = entry.path;
                _import_extract_materials = true;
                _show_import_mesh_dialog = true;
              } else {
                // Same resolution requirement as above — entry.path is relative to assets_directory().
                entry.id = assets_module.import(project.assets_directory() / entry.path);
                state.select_asset(entry.id, entry.path, entry.kind);
              }
            } else if (entry.kind == asset_kind::prefab) {
              // Not importable (no cook step), but still a real manifest-registered, uuid-bearing
              // asset -- unlike scene/script below, it needs a resolved id both for the Inspector
              // and for a drag started from this tile (asset_tile_desc::drag_id, set from entry.id
              // right below where this tile is built) to carry a working uuid instead of nil.
              entry.id = assets_module.import(project.assets_directory() / entry.path);
              state.select_asset(entry.id, entry.path, entry.kind);
            } else if (entry.kind == asset_kind::scene) {
              state.select_asset(sbx::math::uuid::nil(), entry.path, asset_kind::scene);
            } else if (entry.kind == asset_kind::script) {
              state.select_asset(sbx::math::uuid::nil(), entry.path, asset_kind::script);
            }

            // A regular click already ran above (double_clicked implies clicked -- see
            // asset_tile.cpp), so entry.id is already resolved by the importable-kind branch.
            if (tile_result.double_clicked && entry.kind == asset_kind::animation_graph) {
              state.request_open_animation_graph_editor(entry.id, entry.path);
            }
          }

          if (entry.is_directory) {
            if (ImGui::BeginDragDropTarget()) {
              if (const auto* payload = ImGui::AcceptDragDropPayload(asset_move_drag_payload_type)) {
                _try_move(state, path_from_move_payload(*payload), entry.path);
              }

              ImGui::EndDragDropTarget();
            }
          }

          if (is_renaming_this) {
            _draw_rename_field(state, _tile_size);
          } else {
            const auto label = truncate_to_width(entry.path.filename().string(), _tile_size);
            const auto label_width = ImGui::CalcTextSize(label.c_str()).x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (_tile_size - label_width) * 0.5f));
            ImGui::TextUnformatted(label.c_str());
          }

          ImGui::EndGroup();

          if (ImGui::BeginPopupContextItem("##tile_context")) {
            const auto target_directory = entry.is_directory ? entry.path : entry.path.parent_path();

            // A plain click-driven alternative to dragging the tile into the Hierarchy — same
            // instantiate_prefab_command the drag path pushes, just triggered from a menu item
            // instead of a drag gesture, so it doesn't depend on drag-and-drop working at all.
            if (entry.kind == asset_kind::prefab && ImGui::MenuItem(ICON_MDI_CUBE_SCAN " Instantiate in Scene")) {
              if (auto prefab = assets_module.load_prefab(entry.id); prefab.is_valid()) {
                auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
                auto& scene = scenes_module.active_scene();

                auto command = std::make_unique<instantiate_prefab_command>(prefab);
                auto* created = command.get();

                state.push_command(scene, std::move(command));
                state.select_node(scene.find(created->id()));
              }
            }

            if (ImGui::BeginMenu(ICON_MDI_FOLDER_PLUS " Create")) {
              _draw_create_menu(state, target_directory);
              ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem(ICON_MDI_PENCIL " Rename")) {
              _begin_rename(entry.path, entry.is_directory);
            }

            if (ImGui::MenuItem(ICON_MDI_CONTENT_DUPLICATE " Duplicate")) {
              _duplicate(state, entry.path, entry.is_directory);
            }

            if (ImGui::MenuItem(ICON_MDI_DELETE " Delete")) {
              _request_delete(entry.path, entry.is_directory);
            }

            ImGui::EndPopup();
          }

          ImGui::PopID();
        }
      }
    }

    // Right-click the empty area of the contents pane (not an entry — see NoOpenOverItems).
    if (ImGui::BeginPopupContextWindow("##asset_browser_context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
      _draw_create_menu(state, _current_directory);
      ImGui::EndPopup();
    }

    ImGui::EndChild();

    ImGui::EndTable();
  }

  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsKeyPressed(ImGuiKey_F2, false)) {
    if (const auto* selected = std::get_if<asset_selection>(&state.current_selection); selected != nullptr) {
      _begin_rename(selected->path, false);
    }
  }

  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
    if (const auto* selected = std::get_if<asset_selection>(&state.current_selection); selected != nullptr) {
      _request_delete(selected->path, false);
    }
  }

  if (_show_import_mesh_dialog) {
    ImGui::OpenPopup("Import Mesh");
    _show_import_mesh_dialog = false;
  }

  if (ImGui::BeginPopupModal("Import Mesh", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Import '%s'", _pending_import_path.filename().string().c_str());
    ImGui::Checkbox("Extract materials to editable .material assets", &_import_extract_materials);
    ImGui::TextDisabled("Recommended. When off, materials are cooked read-only and won't appear in the Asset Browser.");

    if (ImGui::Button("Import")) {
      const auto id = assets_module.import(project.assets_directory() / _pending_import_path);
      assets_module.load_mesh(id, sbx::assets::mesh_import_options{.extract_materials = _import_extract_materials});
      state.select_asset(id, _pending_import_path, asset_kind::mesh);
      _needs_refresh = true; // newly extracted .material files may now be visible in this folder
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  _import_dialog.draw();

  if (_show_import_conflict_dialog) {
    ImGui::OpenPopup("Import Conflict");
    _show_import_conflict_dialog = false;
  }

  if (ImGui::BeginPopupModal("Import Conflict", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("'%s' already exists in this folder.", _import_conflict_destination.filename().string().c_str());

    if (ImGui::Button("Overwrite")) {
      _import_asset_file(state, _import_conflict_source, _import_conflict_destination);
      _import_conflict_unresolved = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Skip")) {
      _import_conflict_unresolved = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel Remaining")) {
      _pending_asset_imports.clear();
      _import_conflict_unresolved = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  if (_show_delete_confirm_dialog) {
    ImGui::OpenPopup("Delete Asset");
    _show_delete_confirm_dialog = false;
  }

  if (ImGui::BeginPopupModal("Delete Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    if (_pending_delete_is_directory) {
      ImGui::Text("Delete folder '%s' and everything inside it?", _pending_delete_path.filename().string().c_str());
    } else {
      ImGui::Text("Delete '%s'?", _pending_delete_path.filename().string().c_str());
    }

    ImGui::TextDisabled("This cannot be undone.");

    if (ImGui::Button(ICON_MDI_DELETE " Delete")) {
      assets_module.delete_asset(project.assets_directory() / _pending_delete_path);

      if (const auto* selected = std::get_if<asset_selection>(&state.current_selection); selected != nullptr && is_ancestor_or_self(_pending_delete_path, selected->path)) {
        state.clear_selection();
      }

      _needs_refresh = true;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  ImGui::End();
}

auto asset_browser_panel::_process_pending_asset_imports(editor_state& state) -> void {
  auto& project = sbx::core::engine::project();

  while (!_pending_asset_imports.empty() && !_import_conflict_unresolved) {
    const auto source = _pending_asset_imports.front();
    _pending_asset_imports.erase(_pending_asset_imports.begin());

    const auto destination = project.assets_directory() / _import_destination_directory / source.filename();

    if (std::filesystem::exists(destination)) {
      _import_conflict_source = source;
      _import_conflict_destination = destination;
      _import_conflict_unresolved = true;
      _show_import_conflict_dialog = true;

      break;
    }

    _import_asset_file(state, source, destination);
  }
}

auto asset_browser_panel::_import_asset_file(editor_state& state, const std::filesystem::path& source, const std::filesystem::path& destination) -> void {
  auto& project = sbx::core::engine::project();
  auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

  std::filesystem::create_directories(destination.parent_path());

  // copy_file throws if source and destination are the same file (e.g. picking a file already
  // inside the current folder via the dialog) — nothing to copy in that case, just (re-)import it.
  auto ec = std::error_code{};
  if (!std::filesystem::equivalent(source, destination, ec)) {
    std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
  }

  const auto relative_path = std::filesystem::relative(destination, project.assets_directory());
  const auto kind = classify_extension(destination.extension());

  if (kind == asset_kind::mesh && !std::filesystem::exists(std::filesystem::path{destination}.concat(".meta"))) {
    // First time this mesh has ever been seen — same "let the user choose extract_materials"
    // detour the per-entry Import path takes, and the same modal handles both (see draw()).
    _pending_import_path = relative_path;
    _import_extract_materials = true;
    _show_import_mesh_dialog = true;
  } else {
    const auto id = assets_module.import(destination);
    state.select_asset(id, relative_path, kind);
  }

  _needs_refresh = true;
}

} // namespace editor
