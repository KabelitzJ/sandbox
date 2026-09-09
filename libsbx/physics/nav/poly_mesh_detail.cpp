// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/poly_mesh_detail.hpp>

namespace sbx::physics {

[[nodiscard]] auto build_poly_mesh_detail(const poly_mesh& pmesh, const compact_heightfield& chf, std::float_t sample_distance, std::float_t sample_max_error, bake_arena& arena) -> poly_mesh_detail {
  static_cast<void>(chf);
  static_cast<void>(sample_distance);
  static_cast<void>(sample_max_error);

  auto detail = poly_mesh_detail{arena.permanent()};

  const auto nvp = pmesh.max_verts_per_poly;

  for (auto i = std::int32_t{0}; i < pmesh.num_polys; ++i) {
    const auto poly = &pmesh.polys[static_cast<std::size_t>(i) * static_cast<std::size_t>(nvp) * 2u];

    auto poly_verts = std::int32_t{0};

    for (auto j = std::int32_t{0}; j < nvp; ++j) {
      if (poly[j] == mesh_null_index) {
        break;
      }

      ++poly_verts;
    }

    const auto vert_base = static_cast<std::uint32_t>(detail.verts.size());
    const auto tri_base = static_cast<std::uint32_t>(detail.tris.size());

    auto centroid = std::array<std::float_t, 3>{0.0f, 0.0f, 0.0f};

    for (auto j = std::int32_t{0}; j < poly_verts; ++j) {
      const auto v = poly[j];

      const auto wx = pmesh.bounds.min().x() + static_cast<std::float_t>(pmesh.verts[static_cast<std::size_t>(v) * 3u + 0u]) * pmesh.cell_size;
      const auto wy = pmesh.bounds.min().y() + static_cast<std::float_t>(pmesh.verts[static_cast<std::size_t>(v) * 3u + 1u]) * pmesh.cell_height;
      const auto wz = pmesh.bounds.min().z() + static_cast<std::float_t>(pmesh.verts[static_cast<std::size_t>(v) * 3u + 2u]) * pmesh.cell_size;

      detail.verts.push_back(std::array<std::float_t, 3>{wx, wy, wz});

      centroid[0] += wx;
      centroid[1] += wy;
      centroid[2] += wz;
    }

    const auto count = static_cast<std::float_t>(poly_verts);
    centroid[0] /= count;
    centroid[1] /= count;
    centroid[2] /= count;

    const auto centroid_index = static_cast<std::uint32_t>(detail.verts.size());
    detail.verts.push_back(centroid);

    for (auto j = std::int32_t{0}; j < poly_verts; ++j) {
      const auto a = vert_base + static_cast<std::uint32_t>(j);
      const auto b = vert_base + static_cast<std::uint32_t>((j + 1) % poly_verts);

      detail.tris.push_back(std::array<std::uint32_t, 3>{a, b, centroid_index});
    }

    const auto tri_count = static_cast<std::uint32_t>(detail.tris.size()) - tri_base;
    const auto vert_count = static_cast<std::uint32_t>(poly_verts) + 1u;

    detail.meshes.push_back(std::array<std::uint32_t, 4>{vert_base, vert_count, tri_base, tri_count});
  }

  return detail;
}

} // namespace sbx::physics
