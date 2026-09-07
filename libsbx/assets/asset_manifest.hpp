// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_ASSETS_ASSET_MANIFEST_HPP_
#define LIBSBX_ASSETS_ASSET_MANIFEST_HPP_

#include <cstdint>
#include <filesystem>
#include <string_view>
#include <unordered_map>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/math/uuid.hpp>

namespace sbx::assets {

/**
 * @brief The asset database: uuid <-> source path, and the manifest of what's been cooked (content
 * hash + cooker version at last cook, for staleness checks).
 *
 * Split out of what used to be asset_cooker so the racy part stays racy-proof by construction: this
 * class holds **no mutex** and must only ever be touched from the main thread. It used to share a
 * mutex-guarded instance with asset_cooker, but that mutex only protected individual map accesses --
 * import()'s check-cache/create-if-missing/insert sequence spanned two lock acquisitions with an
 * unlocked `.meta` file read/write in between, and ensure_loaded() marked the manifest loaded before
 * _load_manifest() actually finished populating it. Both are real, exploitable races the instant a
 * second thread calls in -- which asset_loader's background thread would have been the first thing
 * to actually do. The fix is to never let a second thread touch this state at all: asset_loader owns
 * its own, separate, stateless asset_cooker instance and never sees an asset_manifest reference.
 *
 * asset_residency (and, for the same reason, assets_module::resolve_mesh_collision_data) holds this
 * by reference, resolves whatever a background load needs (source/cooked path, staleness) up front on
 * the main thread, and hands the *result* to asset_loader -- never this object itself.
 */
class asset_manifest final : public utility::noncopyable {

public:

  asset_manifest() = default;

  ~asset_manifest();

  /**
   * @brief Registers an asset by its path and returns its stable UUID.
   * @param path Must be resolvable from the current working directory — not merely relative to
   * the assets directory.
   */
  auto import(const std::filesystem::path& path) -> math::uuid;

  /**
   * @brief Imports every supported asset under a subdirectory.
   * @param root Must be resolvable from the current working directory. Empty (default) is *not*
   * "the whole assets tree" here — pass `project.assets_directory()` explicitly for that.
   */
  auto import_directory(const std::filesystem::path& root = {}) -> void;

  /** @brief The project-relative path an asset was imported from, or empty if unknown. */
  [[nodiscard]] auto path_of(const math::uuid& id) const -> std::filesystem::path;

  /** @brief Loads the manifest from disk on first call; a no-op after that. Idempotent. */
  auto ensure_loaded() -> void;

  [[nodiscard]] static auto absolute(const std::filesystem::path& relative) -> std::filesystem::path;

  [[nodiscard]] static auto relative(const std::filesystem::path& absolute) -> std::filesystem::path;

  /** @brief Where a given asset's cooked cache blob lives, regardless of whether it exists yet. */
  [[nodiscard]] auto cooked_path(const math::uuid& id, std::string_view extension) const -> std::filesystem::path;

  /** @brief Whether `cooked` needs to be (re)produced from `source` — missing, cooker version bumped, or source content changed since the last recorded cook. */
  [[nodiscard]] auto is_cooked_stale(const math::uuid& id, const std::filesystem::path& source, const std::filesystem::path& cooked, std::uint32_t cooker_version) -> bool;

  /** @brief Records that `id` was just (re)cooked at `cooker_version`, against `source`'s current content — persisted immediately. */
  auto record_cook(const math::uuid& id, std::uint32_t cooker_version, const std::filesystem::path& source) -> void;

private:

  // One row of the asset manifest: the durable uuid -> source path index plus the staleness data
  // (content hash + cooker version) recorded at the last cook.
  struct manifest_entry {
    std::filesystem::path path{};      // project source path (same form as _paths)
    std::uint32_t cooker_version{0u};  // cooker that produced the current cooked output (0 = never)
    std::uint64_t source_hash{0u};     // source content hash at last cook
    std::int64_t source_mtime{0};      // source mtime at last cook (fast-path skip)
  }; // struct manifest_entry

  auto _read_or_create_meta(const std::filesystem::path& path) -> math::uuid;

  [[nodiscard]] auto _manifest_path() const -> std::filesystem::path;

  auto _load_manifest() -> void;

  auto _save_manifest() -> void;

  std::unordered_map<std::string, math::uuid> _uuids{};
  std::unordered_map<math::uuid, std::filesystem::path> _paths{}; // project-relative (to assets directory)

  std::unordered_map<math::uuid, manifest_entry> _manifest{};
  bool _manifest_loaded{false};
  bool _manifest_dirty{false};

}; // class asset_manifest

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSET_MANIFEST_HPP_
