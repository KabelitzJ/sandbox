// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_ASSETS_PRIMITIVE_MESHES_HPP_
#define LIBSBX_ASSETS_PRIMITIVE_MESHES_HPP_

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

#include <libsbx/reflection/enum.hpp>

#include <libsbx/math/uuid.hpp>

namespace sbx::assets {

enum class [[=reflection::named]] primitive_mesh_kind : std::uint8_t {
  cube,
  sphere,
  plane,
  capsule,
  cylinder
}; // enum class primitive_mesh_kind

[[nodiscard]] auto primitive_mesh_uuid(const primitive_mesh_kind kind) -> math::uuid;

[[nodiscard]] auto primitive_mesh_name(const primitive_mesh_kind kind) -> std::string_view;

/** @brief The primitive kind @p id refers to, or nullopt if it isn't one of ours. */
[[nodiscard]] auto primitive_mesh_kind_of(const math::uuid& id) -> std::optional<primitive_mesh_kind>;

/** @brief Generates and writes this primitive's cooked mesh cache blob if it isn't already on disk. Cheap and safe to call every time a primitive is requested -- a no-op once it's been baked once. Also ensures @ref default_material_uuid is cooked -- every primitive's single submesh references it. */
auto ensure_primitive_mesh_cooked(const primitive_mesh_kind kind) -> void;

/**
 * @brief The plain grey, untextured material every built-in primitive's submesh is cooked with --
 * a real, resolvable uuid (unlike a transient create_material()'d handle, this one survives scene
 * serialization; see asset_cooker::write_cooked_material) so a Cube dropped in the Hierarchy
 * renders with something reasonable instead of the mesh-import fallback's error magenta.
 */
[[nodiscard]] auto default_material_uuid() -> math::uuid;

/** @brief Generates and writes the default material's cooked cache blob if it isn't already on disk. Cheap and safe to call every time it's needed -- a no-op once it's been baked once. */
auto ensure_default_material_cooked() -> void;

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_PRIMITIVE_MESHES_HPP_
