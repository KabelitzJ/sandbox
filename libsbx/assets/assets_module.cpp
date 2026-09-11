// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/assets/assets_module.hpp>

#include <array>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <libsbx/core/engine.hpp>
#include <libsbx/core/project.hpp>

namespace sbx::assets {

// Matches editor::extension_table's texture set (editor/panels/asset_browser_panel.cpp) --
// materials are the only asset kind referencing another asset by path rather than uuid, and
// only ever a texture, so this is the one extension set move_asset needs to recognize.
auto is_texture_extension(const std::filesystem::path& extension) -> bool {
  const auto ext = extension.string();
  return ext == ".png" || ext == ".jpg" || ext == ".jpeg";
}

assets_module::assets_module()
: _residency{_manifest, _ibl} { }

auto assets_module::import(const std::filesystem::path& path) -> math::uuid {
  return _manifest.import(path);
}

auto assets_module::import_directory(const std::filesystem::path& root) -> void {
  _manifest.import_directory(root);
}

auto assets_module::move_asset(const std::filesystem::path& old_path, const std::filesystem::path& new_path) -> bool {
  auto moved = std::vector<std::pair<std::filesystem::path, std::filesystem::path>>{};

  if (!_manifest.move(old_path, new_path, moved)) {
    return false;
  }

  for (const auto& [old_relative, new_relative] : moved) {
    if (is_texture_extension(old_relative.extension())) {
      _fixup_material_texture_references(old_relative, new_relative);
    }
  }

  return true;
}

auto assets_module::delete_asset(const std::filesystem::path& path) -> void {
  _manifest.remove(path);
}

auto assets_module::_fixup_material_texture_references(const std::filesystem::path& old_relative, const std::filesystem::path& new_relative) -> void {
  const auto& project = core::engine::project();
  const auto assets_directory = project.assets_directory();

  if (!std::filesystem::exists(assets_directory)) {
    return;
  }

  const auto old_slot = old_relative.generic_string();
  const auto new_slot = new_relative.generic_string();

  static constexpr auto texture_slot_keys = std::array{"albedo", "normal", "metallic_roughness", "occlusion", "emissive"};

  for (const auto& entry : std::filesystem::recursive_directory_iterator{assets_directory}) {
    if (!entry.is_regular_file() || entry.path().extension() != ".material") {
      continue;
    }

    auto node = YAML::Node{};

    try {
      node = YAML::LoadFile(entry.path().string());
    } catch (const std::exception&) {
      continue; // unreadable -- leave it alone rather than clobber it
    }

    auto changed = false;

    for (const auto* key : texture_slot_keys) {
      const auto slot = node[key];

      if (slot && slot.IsScalar() && slot.as<std::string>() == old_slot) {
        node[key] = new_slot;
        changed = true;
      }
    }

    if (changed) {
      auto out = std::ofstream{entry.path()};
      out << node;
    }
  }
}

auto assets_module::load_texture(const math::uuid& id, graphics::format format) -> texture_handle {
  return _residency.load_texture(id, format);
}

auto assets_module::load_texture(const std::filesystem::path& path, graphics::format format) -> texture_handle {
  return _residency.load_texture(path, format);
}

auto assets_module::load_font(const math::uuid& id) -> font_handle {
  return _residency.load_font(id);
}

auto assets_module::load_font(const std::filesystem::path& path) -> font_handle {
  return _residency.load_font(path);
}

auto assets_module::load_mesh(const math::uuid& id, const mesh_import_options& options) -> mesh_handle {
  return _residency.load_mesh(id, options);
}

auto assets_module::load_mesh(const std::filesystem::path& path, const mesh_import_options& options) -> mesh_handle {
  return _residency.load_mesh(path, options);
}

auto assets_module::create_mesh(std::vector<vertex> vertices, std::vector<std::uint32_t> indices, std::vector<mesh::submesh> submeshes, const math::volume& bounds) -> mesh_handle {
  return _residency.create_mesh(std::move(vertices), std::move(indices), std::move(submeshes), bounds);
}

auto assets_module::resolve_mesh_collision_data(const math::uuid& id) -> std::optional<cooked_mesh_data> {
  const auto source = _manifest.path_of(id);

  if (source.empty()) {
    return std::nullopt;
  }

  const auto cooked = _manifest.cooked_path(id, ".sbxmsh");
  const auto needs_cook = _manifest.is_cooked_stale(id, source, cooked, mesh_cooker_version);

  auto did_cook = false;
  auto data = _cooker.resolve_mesh(source, id, cooked, needs_cook, did_cook);

  if (did_cook) {
    _manifest.record_cook(id, mesh_cooker_version, source);
  }

  return data;
}

auto assets_module::load_material(const math::uuid& id) -> material_handle {
  return _residency.load_material(id);
}

auto assets_module::load_material(const std::filesystem::path& path) -> material_handle {
  return _residency.load_material(path);
}

auto assets_module::create_material(const material::create_info& create_info) -> material_handle {
  return _residency.create_material(create_info);
}

auto assets_module::update_material(material_handle& material, const material::create_info& create_info) -> void {
  _residency.update_material(material, create_info);
}

auto assets_module::save_material(material_handle& material, const std::filesystem::path& path) -> math::uuid {
  return _residency.save_material(material, path);
}

auto assets_module::load_environment_map(const math::uuid& id) -> environment_map_handle {
  return _residency.load_environment_map(id);
}

auto assets_module::load_environment_map(const std::filesystem::path& path) -> environment_map_handle {
  return _residency.load_environment_map(path);
}

auto assets_module::load_particle_effect(const math::uuid& id) -> particle_effect_handle {
  return _residency.load_particle_effect(id);
}

auto assets_module::load_particle_effect(const std::filesystem::path& path) -> particle_effect_handle {
  return _residency.load_particle_effect(path);
}

auto assets_module::create_particle_effect(const particle_effect::create_info& create_info) -> particle_effect_handle {
  return _residency.create_particle_effect(create_info);
}

auto assets_module::update_particle_effect(particle_effect_handle& effect, const particle_effect::create_info& create_info) -> void {
  _residency.update_particle_effect(effect, create_info);
}

auto assets_module::save_particle_effect(particle_effect_handle& effect, const std::filesystem::path& path) -> math::uuid {
  return _residency.save_particle_effect(effect, path);
}

auto assets_module::load_animation_graph(const math::uuid& id) -> animation_graph_handle {
  return _residency.load_animation_graph(id);
}

auto assets_module::load_animation_graph(const std::filesystem::path& path) -> animation_graph_handle {
  return _residency.load_animation_graph(path);
}

auto assets_module::create_animation_graph(const animation_graph::create_info& create_info) -> animation_graph_handle {
  return _residency.create_animation_graph(create_info);
}

auto assets_module::update_animation_graph(animation_graph_handle& graph, const animation_graph::create_info& create_info) -> void {
  _residency.update_animation_graph(graph, create_info);
}

auto assets_module::save_animation_graph(animation_graph_handle& graph, const std::filesystem::path& path) -> math::uuid {
  return _residency.save_animation_graph(graph, path);
}

auto assets_module::process_uploads(std::uint64_t frame_index) -> void {
  _residency.process_uploads(frame_index);
}

auto assets_module::is_resident(const texture_handle& texture) const -> bool {
  return _residency.is_resident(texture);
}

auto assets_module::is_resident(const mesh_handle& mesh) const -> bool {
  return _residency.is_resident(mesh);
}

auto assets_module::is_resident(const material_handle& material) const -> bool {
  return _residency.is_resident(material);
}

auto assets_module::is_resident(const environment_map_handle& environment) const -> bool {
  return _residency.is_resident(environment);
}

auto assets_module::is_resident(const font_handle& font) const -> bool {
  return _residency.is_resident(font);
}

auto assets_module::path_of(const math::uuid& id) const -> std::filesystem::path {
  return _manifest.path_of(id);
}

} // namespace sbx::assets
