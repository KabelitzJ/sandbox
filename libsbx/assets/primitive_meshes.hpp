// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_ASSETS_PRIMITIVE_MESHES_HPP_
#define LIBSBX_ASSETS_PRIMITIVE_MESHES_HPP_

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

#include <libsbx/math/uuid.hpp>

namespace sbx::assets {

/**
 * @brief Unity-style built-in primitive shapes -- generated procedurally, not imported from a
 * source file. Each has a fixed, hardcoded uuid (@ref primitive_mesh_uuid), so a scene can
 * reference one the same way it references any other mesh, and its cooked cache blob (@ref
 * ensure_primitive_mesh_cooked) is baked once to the same on-disk cache every other mesh uses --
 * see asset_cooker::write_cooked_mesh.
 */
enum class primitive_mesh_kind : std::uint8_t {
  cube,
  sphere,
  plane,
  capsule,
  cylinder
}; // enum class primitive_mesh_kind

inline constexpr auto primitive_mesh_kinds = std::array<primitive_mesh_kind, 5u>{
  primitive_mesh_kind::cube,
  primitive_mesh_kind::sphere,
  primitive_mesh_kind::plane,
  primitive_mesh_kind::capsule,
  primitive_mesh_kind::cylinder
};

[[nodiscard]] auto primitive_mesh_uuid(primitive_mesh_kind kind) -> math::uuid;

[[nodiscard]] auto primitive_mesh_name(primitive_mesh_kind kind) -> std::string_view;

/** @brief The primitive kind @p id refers to, or nullopt if it isn't one of ours. */
[[nodiscard]] auto primitive_mesh_kind_of(const math::uuid& id) -> std::optional<primitive_mesh_kind>;

/** @brief The primitive kind named @p name (case-sensitive, matches primitive_mesh_name), or nullopt. */
[[nodiscard]] auto primitive_mesh_kind_named(std::string_view name) -> std::optional<primitive_mesh_kind>;

/** @brief Generates and writes this primitive's cooked mesh cache blob if it isn't already on disk. Cheap and safe to call every time a primitive is requested -- a no-op once it's been baked once. */
auto ensure_primitive_mesh_cooked(primitive_mesh_kind kind) -> void;

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_PRIMITIVE_MESHES_HPP_
