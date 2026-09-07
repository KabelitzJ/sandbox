// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_ASSETS_ASSET_LOADER_HPP_
#define LIBSBX_ASSETS_ASSET_LOADER_HPP_

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <filesystem>
#include <mutex>
#include <optional>
#include <thread>
#include <variant>
#include <vector>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/math/uuid.hpp>

#include <libsbx/graphics/types.hpp>

#include <libsbx/assets/asset_cooker.hpp>

namespace sbx::assets {

/**
 * @brief Owns the one background thread that turns load requests into raw decoded/parsed data via
 * its own private @ref asset_cooker -- pure CPU/file I/O, no GPU touch, no handle/index/uuid
 * construction. Independent of core::threading_policy/render_thread (that policy only ever governs
 * whether presentation_module's frame-record work runs on a real thread or inline -- every real app
 * sets it to single_threaded, so render_thread never actually spawns one today); this class always
 * starts a real std::thread, unconditionally.
 *
 * asset_residency submits requests via submit(...) and drains each type's result queue once per
 * frame via the matching take_resolved_*(...), on the main thread -- it alone resolves nested asset
 * references (paths -> uuids -> handles, via asset_manifest/load_*) and interprets raw data into
 * records/pending GPU uploads. The worker thread itself never touches asset_manifest (main-thread-
 * only, see its own doc comment), resource_registry, bindless_table, or upload_context.
 */
class asset_loader final : public utility::noncopyable {

public:

  struct texture_request { math::uuid id; graphics::format format; std::filesystem::path source; std::filesystem::path cooked; bool needs_cook{false}; };
  struct mesh_request { math::uuid id; mesh_import_options options; std::filesystem::path source; std::filesystem::path cooked; bool needs_cook{false}; };
  struct font_request { math::uuid id; std::filesystem::path source; std::filesystem::path cooked; bool needs_cook{false}; };

  // source is a hand-authored `.material` file's path, or empty when this id instead names a
  // material cooked as a side effect of a mesh import (asset_cooker::resolve_cooked_material) --
  // mirrors the branch asset_residency::load_material already picks between today.
  struct material_request { math::uuid id; std::filesystem::path source; };

  struct particle_effect_request { math::uuid id; std::filesystem::path source; };
  struct animation_graph_request { math::uuid id; std::filesystem::path source; };

  // No source/cooked path -- both are self-contained, derived-uuid side effects of a mesh cook
  // (asset_cooker::derive_skeleton_uuid/derive_animation_clip_uuid), resolvable from `id` alone.
  struct skeleton_request { math::uuid id; };
  struct animation_clip_request { math::uuid id; };

  struct texture_result { texture_request request; std::optional<pixel_data> data; bool did_cook{false}; };
  struct mesh_result { mesh_request request; std::optional<cooked_mesh_data> data; bool did_cook{false}; };
  struct font_result { font_request request; std::optional<cooked_font_data> data; bool did_cook{false}; };
  struct material_result { material_request request; std::optional<material_description> data; };
  struct particle_effect_result { particle_effect_request request; std::optional<particle_effect_description> data; };
  struct animation_graph_result { animation_graph_request request; std::optional<animation_graph::create_info> data; };
  struct skeleton_result { skeleton_request request; std::optional<std::vector<skeleton::joint>> data; };
  struct animation_clip_result { animation_clip_request request; std::optional<animation_clip_data> data; };

  asset_loader();

  // Calls abort() if it hasn't already run -- a safety net, not the primary shutdown path (that's
  // an explicit abort() call from whoever owns this, so the intent to stop is visible at the call
  // site rather than only implicit in destruction order).
  ~asset_loader();

  auto submit(texture_request request) -> void;
  auto submit(mesh_request request) -> void;
  auto submit(font_request request) -> void;
  auto submit(material_request request) -> void;
  auto submit(particle_effect_request request) -> void;
  auto submit(animation_graph_request request) -> void;
  auto submit(skeleton_request request) -> void;
  auto submit(animation_clip_request request) -> void;

  /**
   * @brief Stops the loader: drops every not-yet-started request, tells the worker to take no
   * more, and joins it. Idempotent -- safe to call explicitly on shutdown and again from the
   * destructor. A resolve already in flight finishes naturally (asset_cooker's file I/O has no
   * cancellation point) but its result is discarded, not queued -- nothing is left around for a
   * caller to drain after abort() returns. submit(...) silently drops new requests once this has
   * been called.
   */
  auto abort() -> void;

  [[nodiscard]] auto take_resolved_textures(std::size_t max_count) -> std::vector<texture_result>;
  [[nodiscard]] auto take_resolved_meshes(std::size_t max_count) -> std::vector<mesh_result>;
  [[nodiscard]] auto take_resolved_fonts(std::size_t max_count) -> std::vector<font_result>;
  [[nodiscard]] auto take_resolved_materials(std::size_t max_count) -> std::vector<material_result>;
  [[nodiscard]] auto take_resolved_particle_effects(std::size_t max_count) -> std::vector<particle_effect_result>;
  [[nodiscard]] auto take_resolved_animation_graphs(std::size_t max_count) -> std::vector<animation_graph_result>;
  [[nodiscard]] auto take_resolved_skeletons(std::size_t max_count) -> std::vector<skeleton_result>;
  [[nodiscard]] auto take_resolved_animation_clips(std::size_t max_count) -> std::vector<animation_clip_result>;

private:

  using request = std::variant<texture_request, mesh_request, font_request, material_request, particle_effect_request, animation_graph_request, skeleton_request, animation_clip_request>;

  auto _worker_loop() -> void;

  auto _resolve(const texture_request& request) -> void;
  auto _resolve(const mesh_request& request) -> void;
  auto _resolve(const font_request& request) -> void;
  auto _resolve(const material_request& request) -> void;
  auto _resolve(const particle_effect_request& request) -> void;
  auto _resolve(const animation_graph_request& request) -> void;
  auto _resolve(const skeleton_request& request) -> void;
  auto _resolve(const animation_clip_request& request) -> void;

  // Stateless after the asset_manifest split (see asset_manifest.hpp) -- owned by value, private to
  // this loader; nothing else needs a reference to it.
  asset_cooker _cooker{};

  // Checked by the worker between/after every item, and by submit() to refuse new work once
  // shutdown has started.
  std::atomic<bool> _aborted{false};

  std::mutex _request_mutex{};
  std::condition_variable _request_condition{};
  std::deque<request> _requests{};

  std::mutex _result_mutex{};
  std::deque<texture_result> _resolved_textures{};
  std::deque<mesh_result> _resolved_meshes{};
  std::deque<font_result> _resolved_fonts{};
  std::deque<material_result> _resolved_materials{};
  std::deque<particle_effect_result> _resolved_particle_effects{};
  std::deque<animation_graph_result> _resolved_animation_graphs{};
  std::deque<skeleton_result> _resolved_skeletons{};
  std::deque<animation_clip_result> _resolved_animation_clips{};

  // Declared LAST -- constructed after everything above (so the worker never observes a partially
  // constructed *this) and started at the very end of the constructor body.
  std::thread _thread{};

}; // class asset_loader

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSET_LOADER_HPP_
