// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/poly_mesh.hpp>

#include <algorithm>
#include <array>
#include <cmath>

namespace sbx::physics {

[[nodiscard]] auto poly_mesh_prev_index(std::int32_t i, std::int32_t n) -> std::int32_t {
  return i - 1 >= 0 ? i - 1 : n - 1;
}

[[nodiscard]] auto poly_mesh_next_index(std::int32_t i, std::int32_t n) -> std::int32_t {
  return i + 1 < n ? i + 1 : 0;
}

[[nodiscard]] auto poly_mesh_area2(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> std::int32_t {
  return (b.x - a.x) * (c.z - a.z) - (c.x - a.x) * (b.z - a.z);
}

[[nodiscard]] auto poly_mesh_is_left(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> bool {
  return poly_mesh_area2(a, b, c) < 0;
}

[[nodiscard]] auto poly_mesh_is_left_on(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> bool {
  return poly_mesh_area2(a, b, c) <= 0;
}

[[nodiscard]] auto poly_mesh_is_collinear(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> bool {
  return poly_mesh_area2(a, b, c) == 0;
}

[[nodiscard]] auto poly_mesh_intersect_prop(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c, const contour_vertex& d) -> bool {
  if (poly_mesh_is_collinear(a, b, c) || poly_mesh_is_collinear(a, b, d) || poly_mesh_is_collinear(c, d, a) || poly_mesh_is_collinear(c, d, b)) {
    return false;
  }

  return (poly_mesh_is_left(a, b, c) != poly_mesh_is_left(a, b, d)) && (poly_mesh_is_left(c, d, a) != poly_mesh_is_left(c, d, b));
}

[[nodiscard]] auto poly_mesh_is_between(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> bool {
  if (!poly_mesh_is_collinear(a, b, c)) {
    return false;
  }

  if (a.x != b.x) {
    return ((a.x <= c.x) && (c.x <= b.x)) || ((a.x >= c.x) && (c.x >= b.x));
  }

  return ((a.z <= c.z) && (c.z <= b.z)) || ((a.z >= c.z) && (c.z >= b.z));
}

[[nodiscard]] auto poly_mesh_segments_intersect(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c, const contour_vertex& d) -> bool {
  if (poly_mesh_intersect_prop(a, b, c, d)) {
    return true;
  }

  return poly_mesh_is_between(a, b, c) || poly_mesh_is_between(a, b, d) || poly_mesh_is_between(c, d, a) || poly_mesh_is_between(c, d, b);
}

[[nodiscard]] auto poly_mesh_vertices_equal(const contour_vertex& a, const contour_vertex& b) -> bool {
  return a.x == b.x && a.z == b.z;
}

inline constexpr auto index_mask = std::int32_t{0x0fffffff};
inline constexpr auto removable_flag = static_cast<std::int32_t>(0x80000000u);

[[nodiscard]] auto diagonalie(std::int32_t i, std::int32_t j, std::int32_t n, const std::pmr::vector<contour_vertex>& verts, const std::pmr::vector<std::int32_t>& indices) -> bool {
  const auto& d0 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(i)] & index_mask)];
  const auto& d1 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(j)] & index_mask)];

  for (auto k = std::int32_t{0}; k < n; ++k) {
    const auto k1 = poly_mesh_next_index(k, n);

    if (!((k == i) || (k1 == i) || (k == j) || (k1 == j))) {
      const auto& p0 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(k)] & index_mask)];
      const auto& p1 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(k1)] & index_mask)];

      if (poly_mesh_vertices_equal(d0, p0) || poly_mesh_vertices_equal(d1, p0) || poly_mesh_vertices_equal(d0, p1) || poly_mesh_vertices_equal(d1, p1)) {
        continue;
      }

      if (poly_mesh_segments_intersect(d0, d1, p0, p1)) {
        return false;
      }
    }
  }

  return true;
}

[[nodiscard]] auto in_cone(std::int32_t i, std::int32_t j, std::int32_t n, const std::pmr::vector<contour_vertex>& verts, const std::pmr::vector<std::int32_t>& indices) -> bool {
  const auto& pi = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(i)] & index_mask)];
  const auto& pj = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(j)] & index_mask)];
  const auto& pi1 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(poly_mesh_next_index(i, n))] & index_mask)];
  const auto& pin1 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(poly_mesh_prev_index(i, n))] & index_mask)];

  if (poly_mesh_is_left_on(pin1, pi, pi1)) {
    return poly_mesh_is_left(pi, pj, pin1) && poly_mesh_is_left(pj, pi, pi1);
  }

  return !(poly_mesh_is_left_on(pi, pj, pi1) && poly_mesh_is_left_on(pj, pi, pin1));
}

[[nodiscard]] auto is_diagonal(std::int32_t i, std::int32_t j, std::int32_t n, const std::pmr::vector<contour_vertex>& verts, const std::pmr::vector<std::int32_t>& indices) -> bool {
  return in_cone(i, j, n, verts, indices) && diagonalie(i, j, n, verts, indices);
}

[[nodiscard]] auto diagonalie_loose(std::int32_t i, std::int32_t j, std::int32_t n, const std::pmr::vector<contour_vertex>& verts, const std::pmr::vector<std::int32_t>& indices) -> bool {
  const auto& d0 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(i)] & index_mask)];
  const auto& d1 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(j)] & index_mask)];

  for (auto k = std::int32_t{0}; k < n; ++k) {
    const auto k1 = poly_mesh_next_index(k, n);

    if (!((k == i) || (k1 == i) || (k == j) || (k1 == j))) {
      const auto& p0 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(k)] & index_mask)];
      const auto& p1 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(k1)] & index_mask)];

      if (poly_mesh_vertices_equal(d0, p0) || poly_mesh_vertices_equal(d1, p0) || poly_mesh_vertices_equal(d0, p1) || poly_mesh_vertices_equal(d1, p1)) {
        continue;
      }

      if (poly_mesh_intersect_prop(d0, d1, p0, p1)) {
        return false;
      }
    }
  }

  return true;
}

[[nodiscard]] auto in_cone_loose(std::int32_t i, std::int32_t j, std::int32_t n, const std::pmr::vector<contour_vertex>& verts, const std::pmr::vector<std::int32_t>& indices) -> bool {
  const auto& pi = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(i)] & index_mask)];
  const auto& pj = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(j)] & index_mask)];
  const auto& pi1 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(poly_mesh_next_index(i, n))] & index_mask)];
  const auto& pin1 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(poly_mesh_prev_index(i, n))] & index_mask)];

  if (poly_mesh_is_left_on(pin1, pi, pi1)) {
    return poly_mesh_is_left_on(pi, pj, pin1) && poly_mesh_is_left_on(pj, pi, pi1);
  }

  return !(poly_mesh_is_left_on(pi, pj, pi1) && poly_mesh_is_left_on(pj, pi, pin1));
}

[[nodiscard]] auto is_diagonal_loose(std::int32_t i, std::int32_t j, std::int32_t n, const std::pmr::vector<contour_vertex>& verts, const std::pmr::vector<std::int32_t>& indices) -> bool {
  return in_cone_loose(i, j, n, verts, indices) && diagonalie_loose(i, j, n, verts, indices);
}

[[nodiscard]] auto triangulate(std::int32_t n, const std::pmr::vector<contour_vertex>& verts, std::pmr::vector<std::int32_t>& indices, std::pmr::vector<std::int32_t>& tris) -> std::int32_t {
  auto num_tris = std::int32_t{0};
  auto dst = std::size_t{0};

  for (auto i = std::int32_t{0}; i < n; ++i) {
    const auto i1 = poly_mesh_next_index(i, n);
    const auto i2 = poly_mesh_next_index(i1, n);

    if (is_diagonal(i, i2, n, verts, indices)) {
      indices[static_cast<std::size_t>(i1)] |= removable_flag;
    }
  }

  while (n > 3) {
    auto min_len = std::int32_t{-1};
    auto min_i = std::int32_t{-1};

    for (auto i = std::int32_t{0}; i < n; ++i) {
      const auto i1 = poly_mesh_next_index(i, n);

      if (indices[static_cast<std::size_t>(i1)] & removable_flag) {
        const auto& p0 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(i)] & index_mask)];
        const auto& p2 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(poly_mesh_next_index(i1, n))] & index_mask)];

        const auto dx = p2.x - p0.x;
        const auto dz = p2.z - p0.z;
        const auto len = dx * dx + dz * dz;

        if (min_len < 0 || len < min_len) {
          min_len = len;
          min_i = i;
        }
      }
    }

    if (min_i == -1) {
      min_len = -1;
      min_i = -1;

      for (auto i = std::int32_t{0}; i < n; ++i) {
        const auto i1 = poly_mesh_next_index(i, n);
        const auto i2 = poly_mesh_next_index(i1, n);

        if (is_diagonal_loose(i, i2, n, verts, indices)) {
          const auto& p0 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(i)] & index_mask)];
          const auto& p2 = verts[static_cast<std::size_t>(indices[static_cast<std::size_t>(poly_mesh_next_index(i2, n))] & index_mask)];

          const auto dx = p2.x - p0.x;
          const auto dz = p2.z - p0.z;
          const auto len = dx * dx + dz * dz;

          if (min_len < 0 || len < min_len) {
            min_len = len;
            min_i = i;
          }
        }
      }

      if (min_i == -1) {
        return -num_tris;
      }
    }

    auto i = min_i;
    auto i1 = poly_mesh_next_index(i, n);
    const auto i2 = poly_mesh_next_index(i1, n);

    tris[dst++] = indices[static_cast<std::size_t>(i)] & index_mask;
    tris[dst++] = indices[static_cast<std::size_t>(i1)] & index_mask;
    tris[dst++] = indices[static_cast<std::size_t>(i2)] & index_mask;
    ++num_tris;

    --n;

    for (auto k = i1; k < n; ++k) {
      indices[static_cast<std::size_t>(k)] = indices[static_cast<std::size_t>(k + 1)];
    }

    if (i1 >= n) {
      i1 = 0;
    }

    i = poly_mesh_prev_index(i1, n);

    if (is_diagonal(poly_mesh_prev_index(i, n), i1, n, verts, indices)) {
      indices[static_cast<std::size_t>(i)] |= removable_flag;
    } else {
      indices[static_cast<std::size_t>(i)] &= index_mask;
    }

    if (is_diagonal(i, poly_mesh_next_index(i1, n), n, verts, indices)) {
      indices[static_cast<std::size_t>(i1)] |= removable_flag;
    } else {
      indices[static_cast<std::size_t>(i1)] &= index_mask;
    }
  }

  tris[dst++] = indices[0] & index_mask;
  tris[dst++] = indices[1] & index_mask;
  tris[dst++] = indices[2] & index_mask;
  ++num_tris;

  return num_tris;
}

[[nodiscard]] auto count_poly_verts(const std::uint16_t* p, std::int32_t nvp) -> std::int32_t {
  for (auto i = std::int32_t{0}; i < nvp; ++i) {
    if (p[i] == mesh_null_index) {
      return i;
    }
  }

  return nvp;
}

[[nodiscard]] auto u_left(const std::uint16_t* a, const std::uint16_t* b, const std::uint16_t* c) -> bool {
  return (static_cast<std::int32_t>(b[0]) - static_cast<std::int32_t>(a[0])) * (static_cast<std::int32_t>(c[2]) - static_cast<std::int32_t>(a[2])) -
    (static_cast<std::int32_t>(c[0]) - static_cast<std::int32_t>(a[0])) * (static_cast<std::int32_t>(b[2]) - static_cast<std::int32_t>(a[2])) < 0;
}

[[nodiscard]] auto get_poly_merge_value(std::uint16_t* pa, std::uint16_t* pb, const std::pmr::vector<std::uint16_t>& verts, std::int32_t& ea, std::int32_t& eb, std::int32_t nvp) -> std::int32_t {
  const auto na = count_poly_verts(pa, nvp);
  const auto nb = count_poly_verts(pb, nvp);

  if (na + nb - 2 > nvp) {
    return -1;
  }

  ea = -1;
  eb = -1;

  for (auto i = std::int32_t{0}; i < na; ++i) {
    auto va0 = pa[i];
    auto va1 = pa[(i + 1) % na];

    if (va0 > va1) {
      std::swap(va0, va1);
    }

    for (auto j = std::int32_t{0}; j < nb; ++j) {
      auto vb0 = pb[j];
      auto vb1 = pb[(j + 1) % nb];

      if (vb0 > vb1) {
        std::swap(vb0, vb1);
      }

      if (va0 == vb0 && va1 == vb1) {
        ea = i;
        eb = j;
        break;
      }
    }
  }

  if (ea == -1 || eb == -1) {
    return -1;
  }

  auto va = pa[(ea + na - 1) % na];
  auto vb = pa[ea];
  auto vc = pb[(eb + 2) % nb];

  if (!u_left(&verts[static_cast<std::size_t>(va) * 3u], &verts[static_cast<std::size_t>(vb) * 3u], &verts[static_cast<std::size_t>(vc) * 3u])) {
    return -1;
  }

  va = pb[(eb + nb - 1) % nb];
  vb = pb[eb];
  vc = pa[(ea + 2) % na];

  if (!u_left(&verts[static_cast<std::size_t>(va) * 3u], &verts[static_cast<std::size_t>(vb) * 3u], &verts[static_cast<std::size_t>(vc) * 3u])) {
    return -1;
  }

  va = pa[ea];
  vb = pa[(ea + 1) % na];

  const auto dx = static_cast<std::int32_t>(verts[static_cast<std::size_t>(va) * 3u + 0u]) - static_cast<std::int32_t>(verts[static_cast<std::size_t>(vb) * 3u + 0u]);
  const auto dz = static_cast<std::int32_t>(verts[static_cast<std::size_t>(va) * 3u + 2u]) - static_cast<std::int32_t>(verts[static_cast<std::size_t>(vb) * 3u + 2u]);

  return dx * dx + dz * dz;
}

auto merge_poly_verts(std::uint16_t* pa, std::uint16_t* pb, std::int32_t ea, std::int32_t eb, std::pmr::vector<std::uint16_t>& tmp, std::int32_t nvp) -> void {
  const auto na = count_poly_verts(pa, nvp);
  const auto nb = count_poly_verts(pb, nvp);

  std::fill(tmp.begin(), tmp.begin() + nvp, mesh_null_index);

  auto n = std::int32_t{0};

  for (auto i = std::int32_t{0}; i < na - 1; ++i) {
    tmp[static_cast<std::size_t>(n++)] = pa[(ea + 1 + i) % na];
  }

  for (auto i = std::int32_t{0}; i < nb - 1; ++i) {
    tmp[static_cast<std::size_t>(n++)] = pb[(eb + 1 + i) % nb];
  }

  std::copy(tmp.begin(), tmp.begin() + nvp, pa);
}

inline constexpr auto vertex_bucket_count = std::int32_t{1 << 12};

[[nodiscard]] auto compute_vertex_hash(std::int32_t x, std::int32_t y, std::int32_t z) -> std::int32_t {
  constexpr auto h1 = std::uint32_t{0x8da6b343};
  constexpr auto h2 = std::uint32_t{0xd8163841};
  constexpr auto h3 = std::uint32_t{0xcb1ab31f};

  const auto n = h1 * static_cast<std::uint32_t>(x) + h2 * static_cast<std::uint32_t>(y) + h3 * static_cast<std::uint32_t>(z);

  return static_cast<std::int32_t>(n & static_cast<std::uint32_t>(vertex_bucket_count - 1));
}

[[nodiscard]] auto add_vertex(std::uint16_t x, std::uint16_t y, std::uint16_t z, std::pmr::vector<std::uint16_t>& verts, std::pmr::vector<std::int32_t>& first_vert, std::pmr::vector<std::int32_t>& next_vert, std::int32_t& nv) -> std::uint16_t {
  const auto bucket = compute_vertex_hash(x, 0, z);
  auto i = first_vert[static_cast<std::size_t>(bucket)];

  while (i != -1) {
    const auto v = &verts[static_cast<std::size_t>(i) * 3u];

    if (v[0] == x && std::abs(static_cast<std::int32_t>(v[1]) - static_cast<std::int32_t>(y)) <= 2 && v[2] == z) {
      return static_cast<std::uint16_t>(i);
    }

    i = next_vert[static_cast<std::size_t>(i)];
  }

  i = nv;
  ++nv;

  verts[static_cast<std::size_t>(i) * 3u + 0u] = x;
  verts[static_cast<std::size_t>(i) * 3u + 1u] = y;
  verts[static_cast<std::size_t>(i) * 3u + 2u] = z;

  next_vert[static_cast<std::size_t>(i)] = first_vert[static_cast<std::size_t>(bucket)];
  first_vert[static_cast<std::size_t>(bucket)] = i;

  return static_cast<std::uint16_t>(i);
}

struct mesh_edge {
  std::array<std::uint16_t, 2> vert{};
  std::array<std::uint16_t, 2> poly_edge{};
  std::array<std::uint16_t, 2> poly{};
}; // struct mesh_edge

auto build_mesh_adjacency(std::pmr::vector<std::uint16_t>& polys, std::int32_t num_polys, std::int32_t num_verts, std::int32_t verts_per_poly, std::pmr::memory_resource* temp_resource) -> void {
  const auto max_edge_count = num_polys * verts_per_poly;

  auto first_edge = std::pmr::vector<std::uint16_t>(static_cast<std::size_t>(num_verts) + static_cast<std::size_t>(max_edge_count), mesh_null_index, temp_resource);
  const auto next_edge_offset = static_cast<std::size_t>(num_verts);

  auto edge_count = std::int32_t{0};
  auto edges = std::pmr::vector<mesh_edge>(static_cast<std::size_t>(max_edge_count), temp_resource);

  for (auto i = std::int32_t{0}; i < num_polys; ++i) {
    auto t = &polys[static_cast<std::size_t>(i) * static_cast<std::size_t>(verts_per_poly) * 2u];

    for (auto j = std::int32_t{0}; j < verts_per_poly; ++j) {
      if (t[j] == mesh_null_index) {
        break;
      }

      const auto v0 = t[j];
      const auto v1 = (j + 1 >= verts_per_poly || t[j + 1] == mesh_null_index) ? t[0] : t[j + 1];

      if (v0 < v1) {
        auto& edge = edges[static_cast<std::size_t>(edge_count)];
        edge.vert = {v0, v1};
        edge.poly = {static_cast<std::uint16_t>(i), static_cast<std::uint16_t>(i)};
        edge.poly_edge = {static_cast<std::uint16_t>(j), std::uint16_t{0}};

        first_edge[next_edge_offset + static_cast<std::size_t>(edge_count)] = first_edge[static_cast<std::size_t>(v0)];
        first_edge[static_cast<std::size_t>(v0)] = static_cast<std::uint16_t>(edge_count);
        ++edge_count;
      }
    }
  }

  for (auto i = std::int32_t{0}; i < num_polys; ++i) {
    auto t = &polys[static_cast<std::size_t>(i) * static_cast<std::size_t>(verts_per_poly) * 2u];

    for (auto j = std::int32_t{0}; j < verts_per_poly; ++j) {
      if (t[j] == mesh_null_index) {
        break;
      }

      const auto v0 = t[j];
      const auto v1 = (j + 1 >= verts_per_poly || t[j + 1] == mesh_null_index) ? t[0] : t[j + 1];

      if (v0 > v1) {
        for (auto e = first_edge[static_cast<std::size_t>(v1)]; e != mesh_null_index; e = first_edge[next_edge_offset + static_cast<std::size_t>(e)]) {
          auto& edge = edges[static_cast<std::size_t>(e)];

          if (edge.vert[1] == v0 && edge.poly[0] == edge.poly[1]) {
            edge.poly[1] = static_cast<std::uint16_t>(i);
            edge.poly_edge[1] = static_cast<std::uint16_t>(j);
            break;
          }
        }
      }
    }
  }

  for (auto i = std::int32_t{0}; i < edge_count; ++i) {
    const auto& edge = edges[static_cast<std::size_t>(i)];

    if (edge.poly[0] != edge.poly[1]) {
      auto p0 = &polys[static_cast<std::size_t>(edge.poly[0]) * static_cast<std::size_t>(verts_per_poly) * 2u];
      auto p1 = &polys[static_cast<std::size_t>(edge.poly[1]) * static_cast<std::size_t>(verts_per_poly) * 2u];

      p0[verts_per_poly + edge.poly_edge[0]] = edge.poly[1];
      p1[verts_per_poly + edge.poly_edge[1]] = edge.poly[0];
    }
  }
}

[[nodiscard]] auto build_poly_mesh(const contour_set& contours, std::int32_t max_verts_per_poly, bake_arena& arena) -> poly_mesh {
  auto mesh = poly_mesh{arena.permanent()};

  mesh.bounds = contours.bounds;
  mesh.cell_size = contours.cell_size;
  mesh.cell_height = contours.cell_height;
  mesh.border_size = contours.border_size;
  mesh.max_edge_error = contours.max_error;
  mesh.max_verts_per_poly = max_verts_per_poly;

  const auto nvp = max_verts_per_poly;

  auto max_vertices = std::int32_t{0};
  auto max_tris = std::int32_t{0};
  auto max_verts_per_cont = std::int32_t{0};

  for (const auto& cont : contours.contours) {
    if (cont.verts.size() < 3u) {
      continue;
    }

    max_vertices += static_cast<std::int32_t>(cont.verts.size());
    max_tris += static_cast<std::int32_t>(cont.verts.size()) - 2;
    max_verts_per_cont = std::max(max_verts_per_cont, static_cast<std::int32_t>(cont.verts.size()));
  }

  if (max_vertices == 0) {
    return mesh;
  }

  mesh.verts.assign(static_cast<std::size_t>(max_vertices) * 3u, std::uint16_t{0});
  mesh.polys.assign(static_cast<std::size_t>(max_tris) * static_cast<std::size_t>(nvp) * 2u, mesh_null_index);
  mesh.regions.assign(static_cast<std::size_t>(max_tris), std::uint16_t{0});
  mesh.areas.assign(static_cast<std::size_t>(max_tris), std::uint8_t{0});

  mesh.num_verts = 0;
  mesh.num_polys = 0;

  auto next_vert = std::pmr::vector<std::int32_t>(static_cast<std::size_t>(max_vertices), 0, arena.temp());
  auto first_vert = std::pmr::vector<std::int32_t>(static_cast<std::size_t>(vertex_bucket_count), -1, arena.temp());

  auto indices = std::pmr::vector<std::int32_t>(static_cast<std::size_t>(max_verts_per_cont), arena.temp());
  auto tris = std::pmr::vector<std::int32_t>(static_cast<std::size_t>(max_verts_per_cont) * 3u, arena.temp());
  auto polys = std::pmr::vector<std::uint16_t>((static_cast<std::size_t>(max_verts_per_cont) + 1u) * static_cast<std::size_t>(nvp), mesh_null_index, arena.temp());
  auto tmp_poly = std::pmr::vector<std::uint16_t>(static_cast<std::size_t>(nvp), arena.temp());

  for (const auto& cont : contours.contours) {
    if (cont.verts.size() < 3u) {
      continue;
    }

    const auto cont_nverts = static_cast<std::int32_t>(cont.verts.size());

    for (auto j = std::int32_t{0}; j < cont_nverts; ++j) {
      indices[static_cast<std::size_t>(j)] = j;
    }

    auto num_tris = triangulate(cont_nverts, cont.verts, indices, tris);

    if (num_tris <= 0) {
      num_tris = -num_tris;
    }

    for (auto j = std::int32_t{0}; j < cont_nverts; ++j) {
      const auto& v = cont.verts[static_cast<std::size_t>(j)];

      indices[static_cast<std::size_t>(j)] = add_vertex(static_cast<std::uint16_t>(v.x), static_cast<std::uint16_t>(v.y), static_cast<std::uint16_t>(v.z), mesh.verts, first_vert, next_vert, mesh.num_verts);
    }

    auto npolys = std::int32_t{0};
    std::fill(polys.begin(), polys.begin() + static_cast<std::ptrdiff_t>(max_verts_per_cont) * nvp, mesh_null_index);

    for (auto j = std::int32_t{0}; j < num_tris; ++j) {
      const auto t0 = tris[static_cast<std::size_t>(j) * 3u + 0u];
      const auto t1 = tris[static_cast<std::size_t>(j) * 3u + 1u];
      const auto t2 = tris[static_cast<std::size_t>(j) * 3u + 2u];

      if (t0 != t1 && t0 != t2 && t1 != t2) {
        polys[static_cast<std::size_t>(npolys) * static_cast<std::size_t>(nvp) + 0u] = static_cast<std::uint16_t>(indices[static_cast<std::size_t>(t0)]);
        polys[static_cast<std::size_t>(npolys) * static_cast<std::size_t>(nvp) + 1u] = static_cast<std::uint16_t>(indices[static_cast<std::size_t>(t1)]);
        polys[static_cast<std::size_t>(npolys) * static_cast<std::size_t>(nvp) + 2u] = static_cast<std::uint16_t>(indices[static_cast<std::size_t>(t2)]);
        ++npolys;
      }
    }

    if (npolys == 0) {
      continue;
    }

    if (nvp > 3) {
      while (true) {
        auto best_merge_val = std::int32_t{0};
        auto best_pa = std::int32_t{0};
        auto best_pb = std::int32_t{0};
        auto best_ea = std::int32_t{0};
        auto best_eb = std::int32_t{0};

        for (auto j = std::int32_t{0}; j < npolys - 1; ++j) {
          auto pj = &polys[static_cast<std::size_t>(j) * static_cast<std::size_t>(nvp)];

          for (auto k = j + 1; k < npolys; ++k) {
            auto pk = &polys[static_cast<std::size_t>(k) * static_cast<std::size_t>(nvp)];

            auto ea = std::int32_t{0};
            auto eb = std::int32_t{0};
            const auto v = get_poly_merge_value(pj, pk, mesh.verts, ea, eb, nvp);

            if (v > best_merge_val) {
              best_merge_val = v;
              best_pa = j;
              best_pb = k;
              best_ea = ea;
              best_eb = eb;
            }
          }
        }

        if (best_merge_val > 0) {
          auto pa = &polys[static_cast<std::size_t>(best_pa) * static_cast<std::size_t>(nvp)];
          auto pb = &polys[static_cast<std::size_t>(best_pb) * static_cast<std::size_t>(nvp)];

          merge_poly_verts(pa, pb, best_ea, best_eb, tmp_poly, nvp);

          auto last_poly = &polys[static_cast<std::size_t>(npolys - 1) * static_cast<std::size_t>(nvp)];

          if (pb != last_poly) {
            std::copy(last_poly, last_poly + nvp, pb);
          }

          --npolys;
        } else {
          break;
        }
      }
    }

    for (auto j = std::int32_t{0}; j < npolys; ++j) {
      auto p = &mesh.polys[static_cast<std::size_t>(mesh.num_polys) * static_cast<std::size_t>(nvp) * 2u];
      auto q = &polys[static_cast<std::size_t>(j) * static_cast<std::size_t>(nvp)];

      std::copy(q, q + nvp, p);

      mesh.regions[static_cast<std::size_t>(mesh.num_polys)] = cont.region_id;
      mesh.areas[static_cast<std::size_t>(mesh.num_polys)] = cont.area;
      ++mesh.num_polys;
    }
  }

  build_mesh_adjacency(mesh.polys, mesh.num_polys, mesh.num_verts, nvp, arena.temp());

  mesh.verts.resize(static_cast<std::size_t>(mesh.num_verts) * 3u);
  mesh.polys.resize(static_cast<std::size_t>(mesh.num_polys) * static_cast<std::size_t>(nvp) * 2u);
  mesh.regions.resize(static_cast<std::size_t>(mesh.num_polys));
  mesh.areas.resize(static_cast<std::size_t>(mesh.num_polys));
  mesh.flags.assign(static_cast<std::size_t>(mesh.num_polys), std::uint16_t{0});

  return mesh;
}

} // namespace sbx::physics
