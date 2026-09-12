// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/poly_mesh_detail.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>

namespace sbx::physics {

inline constexpr auto unset_height = std::uint16_t{0xffff};
inline constexpr auto max_verts = std::int32_t{127};
inline constexpr auto max_tris = std::int32_t{255};
inline constexpr auto max_verts_per_edge = std::int32_t{32};
inline constexpr auto retract_size = std::int32_t{256};
inline constexpr auto ev_undef = std::int32_t{-1};
inline constexpr auto ev_hull = std::int32_t{-2};

struct height_patch {
  std::int32_t xmin{0};
  std::int32_t ymin{0};
  std::int32_t width{0};
  std::int32_t height{0};
  std::pmr::vector<std::uint16_t> data;
}; // struct height_patch

[[nodiscard]] auto vdot2(const std::float_t* a, const std::float_t* b) -> std::float_t {
  return a[0] * b[0] + a[2] * b[2];
}

[[nodiscard]] auto vdist_sq2(const std::float_t* p, const std::float_t* q) -> std::float_t {
  const auto dx = q[0] - p[0];
  const auto dz = q[2] - p[2];
  return dx * dx + dz * dz;
}

[[nodiscard]] auto vdist2(const std::float_t* p, const std::float_t* q) -> std::float_t {
  return std::sqrt(vdist_sq2(p, q));
}

[[nodiscard]] auto vcross2(const std::float_t* p1, const std::float_t* p2, const std::float_t* p3) -> std::float_t {
  const auto u1 = p2[0] - p1[0];
  const auto v1 = p2[2] - p1[2];
  const auto u2 = p3[0] - p1[0];
  const auto v2 = p3[2] - p1[2];
  return u1 * v2 - v1 * u2;
}

auto circum_circle(const std::float_t* p1, const std::float_t* p2, const std::float_t* p3, std::float_t* c, std::float_t& r) -> bool {
  constexpr auto eps = 1e-6f;

  const auto v1 = std::array<std::float_t, 3>{0.0f, 0.0f, 0.0f};
  const auto v2 = std::array<std::float_t, 3>{p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]};
  const auto v3 = std::array<std::float_t, 3>{p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2]};

  const auto cp = vcross2(v1.data(), v2.data(), v3.data());

  if (std::abs(cp) > eps) {
    const auto v1_sq = vdot2(v1.data(), v1.data());
    const auto v2_sq = vdot2(v2.data(), v2.data());
    const auto v3_sq = vdot2(v3.data(), v3.data());

    c[0] = (v1_sq * (v2[2] - v3[2]) + v2_sq * (v3[2] - v1[2]) + v3_sq * (v1[2] - v2[2])) / (2.0f * cp);
    c[1] = 0.0f;
    c[2] = (v1_sq * (v3[0] - v2[0]) + v2_sq * (v1[0] - v3[0]) + v3_sq * (v2[0] - v1[0])) / (2.0f * cp);
    r = vdist2(c, v1.data());
    c[0] += p1[0];
    c[1] += p1[1];
    c[2] += p1[2];
    return true;
  }

  c[0] = p1[0];
  c[1] = p1[1];
  c[2] = p1[2];
  r = 0.0f;
  return false;
}

[[nodiscard]] auto dist_pt_tri(const std::float_t* p, const std::float_t* a, const std::float_t* b, const std::float_t* c) -> std::float_t {
  const auto v0 = std::array<std::float_t, 3>{c[0] - a[0], c[1] - a[1], c[2] - a[2]};
  const auto v1 = std::array<std::float_t, 3>{b[0] - a[0], b[1] - a[1], b[2] - a[2]};
  const auto v2 = std::array<std::float_t, 3>{p[0] - a[0], p[1] - a[1], p[2] - a[2]};

  const auto dot00 = vdot2(v0.data(), v0.data());
  const auto dot01 = vdot2(v0.data(), v1.data());
  const auto dot02 = vdot2(v0.data(), v2.data());
  const auto dot11 = vdot2(v1.data(), v1.data());
  const auto dot12 = vdot2(v1.data(), v2.data());

  const auto inv_denom = 1.0f / (dot00 * dot11 - dot01 * dot01);
  const auto u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
  const auto v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

  constexpr auto eps = 1e-4f;

  if (u >= -eps && v >= -eps && (u + v) <= 1.0f + eps) {
    const auto y = a[1] + v0[1] * u + v1[1] * v;
    return std::abs(y - p[1]);
  }

  return std::numeric_limits<std::float_t>::max();
}

[[nodiscard]] auto distance_pt_seg(const std::float_t* pt, const std::float_t* p, const std::float_t* q) -> std::float_t {
  const auto pqx = q[0] - p[0];
  const auto pqy = q[1] - p[1];
  const auto pqz = q[2] - p[2];

  const auto d = pqx * pqx + pqy * pqy + pqz * pqz;
  auto t = pqx * (pt[0] - p[0]) + pqy * (pt[1] - p[1]) + pqz * (pt[2] - p[2]);

  if (d > 0.0f) { t /= d; }
  t = std::clamp(t, 0.0f, 1.0f);

  const auto dx = p[0] + t * pqx - pt[0];
  const auto dy = p[1] + t * pqy - pt[1];
  const auto dz = p[2] + t * pqz - pt[2];

  return dx * dx + dy * dy + dz * dz;
}

[[nodiscard]] auto distance_pt_seg_2d(const std::float_t* pt, const std::float_t* p, const std::float_t* q) -> std::float_t {
  const auto pqx = q[0] - p[0];
  const auto pqz = q[2] - p[2];

  const auto d = pqx * pqx + pqz * pqz;
  auto t = pqx * (pt[0] - p[0]) + pqz * (pt[2] - p[2]);

  if (d > 0.0f) { t /= d; }
  t = std::clamp(t, 0.0f, 1.0f);

  const auto dx = p[0] + t * pqx - pt[0];
  const auto dz = p[2] + t * pqz - pt[2];

  return dx * dx + dz * dz;
}

[[nodiscard]] auto dist_to_tri_mesh(const std::float_t* p, const std::float_t* verts, const std::int32_t* tris, std::int32_t ntris) -> std::float_t {
  auto dmin = std::numeric_limits<std::float_t>::max();

  for (auto i = std::int32_t{0}; i < ntris; ++i) {
    const auto* va = &verts[tris[i * 4 + 0] * 3];
    const auto* vb = &verts[tris[i * 4 + 1] * 3];
    const auto* vc = &verts[tris[i * 4 + 2] * 3];
    dmin = std::min(dmin, dist_pt_tri(p, va, vb, vc));
  }

  return dmin == std::numeric_limits<std::float_t>::max() ? -1.0f : dmin;
}

[[nodiscard]] auto dist_to_poly(std::int32_t nvert, const std::float_t* verts, const std::float_t* p) -> std::float_t {
  auto dmin = std::numeric_limits<std::float_t>::max();
  auto c = false;

  for (auto i = std::int32_t{0}, j = nvert - 1; i < nvert; j = i++) {
    const auto* vi = &verts[i * 3];
    const auto* vj = &verts[j * 3];

    if (((vi[2] > p[2]) != (vj[2] > p[2])) && (p[0] < (vj[0] - vi[0]) * (p[2] - vi[2]) / (vj[2] - vi[2]) + vi[0])) {
      c = !c;
    }

    dmin = std::min(dmin, distance_pt_seg_2d(p, vj, vi));
  }

  return c ? -dmin : dmin;
}

[[nodiscard]] auto get_height(std::float_t fx, std::float_t fy, std::float_t fz, std::float_t ics, std::float_t ch, std::int32_t radius, const height_patch& hp) -> std::uint16_t {
  auto ix = static_cast<std::int32_t>(std::floor(fx * ics + 0.01f));
  auto iz = static_cast<std::int32_t>(std::floor(fz * ics + 0.01f));
  ix = std::clamp(ix - hp.xmin, 0, hp.width - 1);
  iz = std::clamp(iz - hp.ymin, 0, hp.height - 1);

  auto h = hp.data[static_cast<std::size_t>(ix + iz * hp.width)];

  if (h == unset_height) {
    auto x = std::int32_t{1};
    auto z = std::int32_t{0};
    auto dx = std::int32_t{1};
    auto dz = std::int32_t{0};
    const auto max_size = radius * 2 + 1;
    const auto max_iter = max_size * max_size - 1;

    auto next_ring_iter_start = std::int32_t{8};
    auto next_ring_iters = std::int32_t{16};

    auto dmin = std::numeric_limits<std::float_t>::max();

    for (auto i = std::int32_t{0}; i < max_iter; ++i) {
      const auto nx = ix + x;
      const auto nz = iz + z;

      if (nx >= 0 && nz >= 0 && nx < hp.width && nz < hp.height) {
        const auto nh = hp.data[static_cast<std::size_t>(nx + nz * hp.width)];

        if (nh != unset_height) {
          const auto d = std::abs(static_cast<std::float_t>(nh) * ch - fy);

          if (d < dmin) {
            h = nh;
            dmin = d;
          }
        }
      }

      if (i + 1 == next_ring_iter_start) {
        if (h != unset_height) { break; }
        next_ring_iter_start += next_ring_iters;
        next_ring_iters += 8;
      }

      if ((x == z) || ((x < 0) && (x == -z)) || ((x > 0) && (x == 1 - z))) {
        const auto tmp = dx;
        dx = -dz;
        dz = tmp;
      }

      x += dx;
      z += dz;
    }
  }

  return h;
}

[[nodiscard]] auto find_edge(const std::int32_t* edges, std::int32_t nedges, std::int32_t s, std::int32_t t) -> std::int32_t {
  for (auto i = std::int32_t{0}; i < nedges; ++i) {
    const auto* e = &edges[i * 4];
    if ((e[0] == s && e[1] == t) || (e[0] == t && e[1] == s)) { return i; }
  }
  return ev_undef;
}

auto add_edge(std::int32_t* edges, std::int32_t& nedges, std::int32_t max_edges, std::int32_t s, std::int32_t t, std::int32_t l, std::int32_t r) -> std::int32_t {
  if (nedges >= max_edges) { return ev_undef; }

  if (find_edge(edges, nedges, s, t) != ev_undef) { return ev_undef; }

  auto* edge = &edges[nedges * 4];
  edge[0] = s;
  edge[1] = t;
  edge[2] = l;
  edge[3] = r;
  return nedges++;
}

auto update_left_face(std::int32_t* e, std::int32_t s, std::int32_t t, std::int32_t f) -> void {
  if (e[0] == s && e[1] == t && e[2] == ev_undef) {
    e[2] = f;
  } else if (e[1] == s && e[0] == t && e[3] == ev_undef) {
    e[3] = f;
  }
}

[[nodiscard]] auto overlap_seg_seg_2d(const std::float_t* a, const std::float_t* b, const std::float_t* c, const std::float_t* d) -> bool {
  const auto a1 = vcross2(a, b, d);
  const auto a2 = vcross2(a, b, c);

  if (a1 * a2 < 0.0f) {
    const auto a3 = vcross2(c, d, a);
    const auto a4 = a3 + a2 - a1;
    if (a3 * a4 < 0.0f) { return true; }
  }

  return false;
}

[[nodiscard]] auto overlap_edges(const std::float_t* pts, const std::int32_t* edges, std::int32_t nedges, std::int32_t s1, std::int32_t t1) -> bool {
  for (auto i = std::int32_t{0}; i < nedges; ++i) {
    const auto s0 = edges[i * 4 + 0];
    const auto t0 = edges[i * 4 + 1];

    if (s0 == s1 || s0 == t1 || t0 == s1 || t0 == t1) { continue; }
    if (overlap_seg_seg_2d(&pts[s0 * 3], &pts[t0 * 3], &pts[s1 * 3], &pts[t1 * 3])) { return true; }
  }
  return false;
}

auto complete_facet(const std::float_t* pts, std::int32_t npts, std::int32_t* edges, std::int32_t& nedges, std::int32_t max_edges, std::int32_t& nfaces, std::int32_t e) -> void {
  const auto* edge = &edges[e * 4];

  auto s = std::int32_t{0};
  auto t = std::int32_t{0};

  if (edge[2] == ev_undef) {
    s = edge[0];
    t = edge[1];
  } else if (edge[3] == ev_undef) {
    s = edge[1];
    t = edge[0];
  } else {
    return;
  }

  auto pt = npts;
  auto c = std::array<std::float_t, 3>{0.0f, 0.0f, 0.0f};
  auto r = -1.0f;

  for (auto u = std::int32_t{0}; u < npts; ++u) {
    if (u == s || u == t) { continue; }

    constexpr auto eps = 1e-5f;

    if (vcross2(&pts[s * 3], &pts[t * 3], &pts[u * 3]) > eps) {
      if (r < 0.0f) {
        pt = u;
        circum_circle(&pts[s * 3], &pts[t * 3], &pts[u * 3], c.data(), r);
        continue;
      }

      const auto d = vdist2(c.data(), &pts[u * 3]);
      constexpr auto tol = 0.001f;

      if (d > r * (1.0f + tol)) {
        continue;
      } else if (d < r * (1.0f - tol)) {
        pt = u;
        circum_circle(&pts[s * 3], &pts[t * 3], &pts[u * 3], c.data(), r);
      } else {
        if (overlap_edges(pts, edges, nedges, s, u)) { continue; }
        if (overlap_edges(pts, edges, nedges, t, u)) { continue; }
        pt = u;
        circum_circle(&pts[s * 3], &pts[t * 3], &pts[u * 3], c.data(), r);
      }
    }
  }

  if (pt < npts) {
    update_left_face(&edges[e * 4], s, t, nfaces);

    auto edge_index = find_edge(edges, nedges, pt, s);
    if (edge_index == ev_undef) {
      add_edge(edges, nedges, max_edges, pt, s, nfaces, ev_undef);
    } else {
      update_left_face(&edges[edge_index * 4], pt, s, nfaces);
    }

    edge_index = find_edge(edges, nedges, t, pt);
    if (edge_index == ev_undef) {
      add_edge(edges, nedges, max_edges, t, pt, nfaces, ev_undef);
    } else {
      update_left_face(&edges[edge_index * 4], t, pt, nfaces);
    }

    ++nfaces;
  } else {
    update_left_face(&edges[e * 4], s, t, ev_hull);
  }
}

auto delaunay_hull(std::int32_t npts, const std::float_t* pts, std::int32_t nhull, const std::int32_t* hull, std::pmr::vector<std::int32_t>& tris, std::pmr::vector<std::int32_t>& edges) -> void {
  auto nfaces = std::int32_t{0};
  auto nedges = std::int32_t{0};
  const auto max_edges = npts * 10;
  edges.assign(static_cast<std::size_t>(max_edges * 4), 0);

  for (auto i = std::int32_t{0}, j = nhull - 1; i < nhull; j = i++) {
    add_edge(edges.data(), nedges, max_edges, hull[j], hull[i], ev_hull, ev_undef);
  }

  auto current_edge = std::int32_t{0};

  while (current_edge < nedges) {
    if (edges[static_cast<std::size_t>(current_edge * 4 + 2)] == ev_undef) {
      complete_facet(pts, npts, edges.data(), nedges, max_edges, nfaces, current_edge);
    }
    if (edges[static_cast<std::size_t>(current_edge * 4 + 3)] == ev_undef) {
      complete_facet(pts, npts, edges.data(), nedges, max_edges, nfaces, current_edge);
    }
    ++current_edge;
  }

  tris.assign(static_cast<std::size_t>(nfaces * 4), -1);

  for (auto i = std::int32_t{0}; i < nedges; ++i) {
    const auto* e = &edges[static_cast<std::size_t>(i * 4)];

    if (e[3] >= 0) {
      auto* face = &tris[static_cast<std::size_t>(e[3] * 4)];
      if (face[0] == -1) {
        face[0] = e[0];
        face[1] = e[1];
      } else if (face[0] == e[1]) {
        face[2] = e[0];
      } else if (face[1] == e[0]) {
        face[2] = e[1];
      }
    }

    if (e[2] >= 0) {
      auto* face = &tris[static_cast<std::size_t>(e[2] * 4)];
      if (face[0] == -1) {
        face[0] = e[1];
        face[1] = e[0];
      } else if (face[0] == e[0]) {
        face[2] = e[1];
      } else if (face[1] == e[1]) {
        face[2] = e[0];
      }
    }
  }

  for (auto i = std::int32_t{0}; i < static_cast<std::int32_t>(tris.size()) / 4; ++i) {
    auto* t = &tris[static_cast<std::size_t>(i * 4)];
    if (t[0] == -1 || t[1] == -1 || t[2] == -1) {
      t[0] = tris[tris.size() - 4];
      t[1] = tris[tris.size() - 3];
      t[2] = tris[tris.size() - 2];
      t[3] = tris[tris.size() - 1];
      tris.resize(tris.size() - 4);
      --i;
    }
  }
}

[[nodiscard]] auto poly_min_extent(const std::float_t* verts, std::int32_t nverts) -> std::float_t {
  auto min_dist = std::numeric_limits<std::float_t>::max();

  for (auto i = std::int32_t{0}; i < nverts; ++i) {
    const auto ni = (i + 1) % nverts;
    const auto* p1 = &verts[i * 3];
    const auto* p2 = &verts[ni * 3];
    auto max_edge_dist = 0.0f;

    for (auto j = std::int32_t{0}; j < nverts; ++j) {
      if (j == i || j == ni) { continue; }
      max_edge_dist = std::max(max_edge_dist, distance_pt_seg_2d(&verts[j * 3], p1, p2));
    }

    min_dist = std::min(min_dist, max_edge_dist);
  }

  return std::sqrt(min_dist);
}

[[nodiscard]] constexpr auto hull_prev_index(std::int32_t i, std::int32_t n) -> std::int32_t {
  return i - 1 >= 0 ? i - 1 : n - 1;
}

[[nodiscard]] constexpr auto hull_next_index(std::int32_t i, std::int32_t n) -> std::int32_t {
  return i + 1 < n ? i + 1 : 0;
}

auto triangulate_hull(const std::float_t* verts, std::int32_t nhull, const std::int32_t* hull, std::int32_t nin, std::pmr::vector<std::int32_t>& tris) -> void {
  auto start = std::int32_t{0};
  auto left = std::int32_t{1};
  auto right = nhull - 1;

  auto dmin = std::numeric_limits<std::float_t>::max();

  for (auto i = std::int32_t{0}; i < nhull; ++i) {
    if (hull[i] >= nin) { continue; }

    const auto pi = hull_prev_index(i, nhull);
    const auto ni = hull_next_index(i, nhull);
    const auto* pv = &verts[hull[pi] * 3];
    const auto* cv = &verts[hull[i] * 3];
    const auto* nv = &verts[hull[ni] * 3];
    const auto d = vdist2(pv, cv) + vdist2(cv, nv) + vdist2(nv, pv);

    if (d < dmin) {
      start = i;
      left = ni;
      right = pi;
      dmin = d;
    }
  }

  tris.push_back(hull[start]);
  tris.push_back(hull[left]);
  tris.push_back(hull[right]);
  tris.push_back(0);

  while (hull_next_index(left, nhull) != right) {
    const auto nleft = hull_next_index(left, nhull);
    const auto nright = hull_prev_index(right, nhull);

    const auto* cvleft = &verts[hull[left] * 3];
    const auto* nvleft = &verts[hull[nleft] * 3];
    const auto* cvright = &verts[hull[right] * 3];
    const auto* nvright = &verts[hull[nright] * 3];

    const auto dleft = vdist2(cvleft, nvleft) + vdist2(nvleft, cvright);
    const auto dright = vdist2(cvright, nvright) + vdist2(cvleft, nvright);

    if (dleft < dright) {
      tris.push_back(hull[left]);
      tris.push_back(hull[nleft]);
      tris.push_back(hull[right]);
      tris.push_back(0);
      left = nleft;
    } else {
      tris.push_back(hull[left]);
      tris.push_back(hull[nright]);
      tris.push_back(hull[right]);
      tris.push_back(0);
      right = nright;
    }
  }
}

[[nodiscard]] auto get_jitter_x(std::int32_t i) -> std::float_t {
  return static_cast<std::float_t>((static_cast<std::uint32_t>(i) * 0x8da6b343u) & 0xffffu) / 65535.0f * 2.0f - 1.0f;
}

[[nodiscard]] auto get_jitter_y(std::int32_t i) -> std::float_t {
  return static_cast<std::float_t>((static_cast<std::uint32_t>(i) * 0xd8163841u) & 0xffffu) / 65535.0f * 2.0f - 1.0f;
}

auto build_poly_detail(const std::float_t* in, std::int32_t nin, std::float_t sample_dist, std::float_t sample_max_error, std::int32_t height_search_radius, const compact_heightfield& chf, const height_patch& hp, std::float_t* verts, std::int32_t& nverts, std::pmr::vector<std::int32_t>& tris, std::pmr::vector<std::int32_t>& edges, std::pmr::vector<std::int32_t>& samples) -> void {
  auto edge = std::array<std::float_t, static_cast<std::size_t>(max_verts_per_edge + 1) * 3u>{};
  auto hull = std::array<std::int32_t, static_cast<std::size_t>(max_verts)>{};
  auto nhull = std::int32_t{0};

  nverts = nin;

  for (auto i = std::int32_t{0}; i < nin; ++i) {
    verts[i * 3 + 0] = in[i * 3 + 0];
    verts[i * 3 + 1] = in[i * 3 + 1];
    verts[i * 3 + 2] = in[i * 3 + 2];
  }

  edges.clear();
  tris.clear();

  const auto cs = chf.cell_size;
  const auto ics = 1.0f / cs;

  const auto min_extent = poly_min_extent(verts, nverts);

  if (sample_dist > 0.0f) {
    for (auto i = std::int32_t{0}, j = nin - 1; i < nin; j = i++) {
      auto vj = &in[j * 3];
      auto vi = &in[i * 3];
      auto swapped = false;

      if (std::abs(vj[0] - vi[0]) < 1e-6f) {
        if (vj[2] > vi[2]) {
          std::swap(vj, vi);
          swapped = true;
        }
      } else {
        if (vj[0] > vi[0]) {
          std::swap(vj, vi);
          swapped = true;
        }
      }

      const auto dx = vi[0] - vj[0];
      const auto dy = vi[1] - vj[1];
      const auto dz = vi[2] - vj[2];
      const auto d = std::sqrt(dx * dx + dz * dz);

      auto nn = 1 + static_cast<std::int32_t>(std::floor(d / sample_dist));
      if (nn >= max_verts_per_edge) { nn = max_verts_per_edge - 1; }
      if (nverts + nn >= max_verts) { nn = max_verts - 1 - nverts; }

      for (auto k = std::int32_t{0}; k <= nn; ++k) {
        const auto u = static_cast<std::float_t>(k) / static_cast<std::float_t>(nn);
        auto* pos = &edge[static_cast<std::size_t>(k * 3)];
        pos[0] = vj[0] + dx * u;
        pos[1] = vj[1] + dy * u;
        pos[2] = vj[2] + dz * u;
        pos[1] = static_cast<std::float_t>(get_height(pos[0], pos[1], pos[2], ics, chf.cell_height, height_search_radius, hp)) * chf.cell_height;
      }

      auto idx = std::array<std::int32_t, static_cast<std::size_t>(max_verts_per_edge)>{0, nn};
      auto nidx = std::int32_t{2};

      for (auto k = std::int32_t{0}; k < nidx - 1; ) {
        const auto a = idx[static_cast<std::size_t>(k)];
        const auto b = idx[static_cast<std::size_t>(k + 1)];
        const auto* va = &edge[static_cast<std::size_t>(a * 3)];
        const auto* vb = &edge[static_cast<std::size_t>(b * 3)];

        auto maxd = 0.0f;
        auto maxi = -1;

        for (auto m = a + 1; m < b; ++m) {
          const auto dev = distance_pt_seg(&edge[static_cast<std::size_t>(m * 3)], va, vb);
          if (dev > maxd) {
            maxd = dev;
            maxi = m;
          }
        }

        if (maxi != -1 && maxd > sample_max_error * sample_max_error) {
          for (auto m = nidx; m > k; --m) {
            idx[static_cast<std::size_t>(m)] = idx[static_cast<std::size_t>(m - 1)];
          }
          idx[static_cast<std::size_t>(k + 1)] = maxi;
          ++nidx;
        } else {
          ++k;
        }
      }

      hull[static_cast<std::size_t>(nhull++)] = j;

      if (swapped) {
        for (auto k = nidx - 2; k > 0; --k) {
          const auto src = static_cast<std::size_t>(idx[static_cast<std::size_t>(k)] * 3);
          verts[nverts * 3 + 0] = edge[src + 0];
          verts[nverts * 3 + 1] = edge[src + 1];
          verts[nverts * 3 + 2] = edge[src + 2];
          hull[static_cast<std::size_t>(nhull++)] = nverts;
          ++nverts;
        }
      } else {
        for (auto k = std::int32_t{1}; k < nidx - 1; ++k) {
          const auto src = static_cast<std::size_t>(idx[static_cast<std::size_t>(k)] * 3);
          verts[nverts * 3 + 0] = edge[src + 0];
          verts[nverts * 3 + 1] = edge[src + 1];
          verts[nverts * 3 + 2] = edge[src + 2];
          hull[static_cast<std::size_t>(nhull++)] = nverts;
          ++nverts;
        }
      }
    }
  }

  if (min_extent < sample_dist * 2.0f) {
    triangulate_hull(verts, nhull, hull.data(), nin, tris);
    return;
  }

  triangulate_hull(verts, nhull, hull.data(), nin, tris);

  if (tris.empty()) {
    return;
  }

  if (sample_dist > 0.0f) {
    auto bmin = std::array<std::float_t, 3>{in[0], in[1], in[2]};
    auto bmax = std::array<std::float_t, 3>{in[0], in[1], in[2]};

    for (auto i = std::int32_t{1}; i < nin; ++i) {
      bmin[0] = std::min(bmin[0], in[i * 3 + 0]);
      bmin[1] = std::min(bmin[1], in[i * 3 + 1]);
      bmin[2] = std::min(bmin[2], in[i * 3 + 2]);
      bmax[0] = std::max(bmax[0], in[i * 3 + 0]);
      bmax[1] = std::max(bmax[1], in[i * 3 + 1]);
      bmax[2] = std::max(bmax[2], in[i * 3 + 2]);
    }

    const auto x0 = static_cast<std::int32_t>(std::floor(bmin[0] / sample_dist));
    const auto x1 = static_cast<std::int32_t>(std::ceil(bmax[0] / sample_dist));
    const auto z0 = static_cast<std::int32_t>(std::floor(bmin[2] / sample_dist));
    const auto z1 = static_cast<std::int32_t>(std::ceil(bmax[2] / sample_dist));

    samples.clear();

    for (auto z = z0; z < z1; ++z) {
      for (auto x = x0; x < x1; ++x) {
        const auto pt = std::array<std::float_t, 3>{
          static_cast<std::float_t>(x) * sample_dist,
          (bmax[1] + bmin[1]) * 0.5f,
          static_cast<std::float_t>(z) * sample_dist
        };

        if (dist_to_poly(nin, in, pt.data()) > -sample_dist * 0.5f) { continue; }

        samples.push_back(x);
        samples.push_back(static_cast<std::int32_t>(get_height(pt[0], pt[1], pt[2], ics, chf.cell_height, height_search_radius, hp)));
        samples.push_back(z);
        samples.push_back(0);
      }
    }

    const auto nsamples = static_cast<std::int32_t>(samples.size()) / 4;

    for (auto iter = std::int32_t{0}; iter < nsamples; ++iter) {
      if (nverts >= max_verts) { break; }

      auto bestpt = std::array<std::float_t, 3>{0.0f, 0.0f, 0.0f};
      auto bestd = 0.0f;
      auto besti = -1;

      for (auto i = std::int32_t{0}; i < nsamples; ++i) {
        const auto* s = &samples[static_cast<std::size_t>(i * 4)];
        if (s[3] != 0) { continue; }

        const auto pt = std::array<std::float_t, 3>{
          static_cast<std::float_t>(s[0]) * sample_dist + get_jitter_x(i) * cs * 0.1f,
          static_cast<std::float_t>(s[1]) * chf.cell_height,
          static_cast<std::float_t>(s[2]) * sample_dist + get_jitter_y(i) * cs * 0.1f
        };

        if (tris.empty()) { continue; }

        const auto d = dist_to_tri_mesh(pt.data(), verts, tris.data(), static_cast<std::int32_t>(tris.size()) / 4);
        if (d < 0.0f) { continue; }

        if (d > bestd) {
          bestd = d;
          besti = i;
          bestpt = pt;
        }
      }

      if (bestd <= sample_max_error || besti == -1) { break; }

      samples[static_cast<std::size_t>(besti * 4 + 3)] = 1;

      verts[nverts * 3 + 0] = bestpt[0];
      verts[nverts * 3 + 1] = bestpt[1];
      verts[nverts * 3 + 2] = bestpt[2];
      ++nverts;

      edges.clear();
      tris.clear();
      delaunay_hull(nverts, verts, nhull, hull.data(), tris, edges);
    }
  }

  const auto ntris = static_cast<std::int32_t>(tris.size()) / 4;
  if (ntris > max_tris) {
    tris.resize(static_cast<std::size_t>(max_tris * 4));
  }
}

auto push3(std::pmr::vector<std::int32_t>& queue, std::int32_t v1, std::int32_t v2, std::int32_t v3) -> void {
  queue.push_back(v1);
  queue.push_back(v2);
  queue.push_back(v3);
}

[[nodiscard]] constexpr auto get_dir_for_offset(std::int32_t x, std::int32_t z) -> std::int32_t {
  constexpr auto dirs = std::array<std::int32_t, 5>{3, 0, -1, 2, 1};
  return dirs[static_cast<std::size_t>(((z + 1) << 1) + x)];
}

auto seed_array_with_poly_center(const compact_heightfield& chf, const std::uint16_t* poly, std::int32_t npoly, const std::uint16_t* verts, std::int32_t bs, height_patch& hp, std::pmr::vector<std::int32_t>& array) -> void {
  constexpr auto offset = std::array<std::int32_t, 18>{0, 0, -1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0};

  auto start_cell_x = std::int32_t{0};
  auto start_cell_y = std::int32_t{0};
  auto start_span_index = std::int32_t{-1};
  auto dmin = static_cast<std::int32_t>(unset_height);

  for (auto j = std::int32_t{0}; j < npoly && dmin > 0; ++j) {
    for (auto k = std::int32_t{0}; k < 9 && dmin > 0; ++k) {
      const auto ax = static_cast<std::int32_t>(verts[poly[j] * 3 + 0]) + offset[static_cast<std::size_t>(k * 2 + 0)];
      const auto ay = static_cast<std::int32_t>(verts[poly[j] * 3 + 1]);
      const auto az = static_cast<std::int32_t>(verts[poly[j] * 3 + 2]) + offset[static_cast<std::size_t>(k * 2 + 1)];

      if (ax < hp.xmin || ax >= hp.xmin + hp.width || az < hp.ymin || az >= hp.ymin + hp.height) { continue; }

      const auto& c = chf.cells[static_cast<std::size_t>((ax + bs) + (az + bs) * chf.width)];

      for (auto i = static_cast<std::int32_t>(c.index), ni = static_cast<std::int32_t>(c.index) + static_cast<std::int32_t>(c.count); i < ni && dmin > 0; ++i) {
        const auto& s = chf.spans[static_cast<std::size_t>(i)];
        const auto d = std::abs(ay - static_cast<std::int32_t>(s.min));

        if (d < dmin) {
          start_cell_x = ax;
          start_cell_y = az;
          start_span_index = i;
          dmin = d;
        }
      }
    }
  }

  auto pcx = std::int32_t{0};
  auto pcy = std::int32_t{0};

  for (auto j = std::int32_t{0}; j < npoly; ++j) {
    pcx += static_cast<std::int32_t>(verts[poly[j] * 3 + 0]);
    pcy += static_cast<std::int32_t>(verts[poly[j] * 3 + 2]);
  }

  pcx /= npoly;
  pcy /= npoly;

  array.clear();
  array.push_back(start_cell_x);
  array.push_back(start_cell_y);
  array.push_back(start_span_index);

  auto dirs = std::array<std::int32_t, 4>{0, 1, 2, 3};
  std::fill(hp.data.begin(), hp.data.end(), std::uint16_t{0});

  auto cx = std::int32_t{-1};
  auto cy = std::int32_t{-1};
  auto ci = std::int32_t{-1};

  while (true) {
    if (array.size() < 3) { break; }

    ci = array.back(); array.pop_back();
    cy = array.back(); array.pop_back();
    cx = array.back(); array.pop_back();

    if (cx == pcx && cy == pcy) { break; }

    const auto direct_dir = (cx == pcx)
      ? get_dir_for_offset(0, pcy > cy ? 1 : -1)
      : get_dir_for_offset(pcx > cx ? 1 : -1, 0);

    std::swap(dirs[static_cast<std::size_t>(direct_dir)], dirs[3]);

    const auto& cs = chf.spans[static_cast<std::size_t>(ci)];

    for (auto i = std::int32_t{0}; i < 4; ++i) {
      const auto dir = dirs[static_cast<std::size_t>(i)];
      if (get_connection(cs, dir) == static_cast<std::int32_t>(not_connected)) { continue; }

      const auto new_x = cx + direction_offset_x(dir);
      const auto new_y = cy + direction_offset_z(dir);

      const auto hpx = new_x - hp.xmin;
      const auto hpy = new_y - hp.ymin;

      if (hpx < 0 || hpx >= hp.width || hpy < 0 || hpy >= hp.height) { continue; }
      if (hp.data[static_cast<std::size_t>(hpx + hpy * hp.width)] != 0) { continue; }

      hp.data[static_cast<std::size_t>(hpx + hpy * hp.width)] = 1;
      array.push_back(new_x);
      array.push_back(new_y);
      array.push_back(static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>((new_x + bs) + (new_y + bs) * chf.width)].index) + get_connection(cs, dir));
    }

    std::swap(dirs[static_cast<std::size_t>(direct_dir)], dirs[3]);
  }

  array.clear();
  array.push_back(cx + bs);
  array.push_back(cy + bs);
  array.push_back(ci);

  std::fill(hp.data.begin(), hp.data.end(), unset_height);

  const auto& seed_span = chf.spans[static_cast<std::size_t>(ci)];
  hp.data[static_cast<std::size_t>(cx - hp.xmin + (cy - hp.ymin) * hp.width)] = seed_span.min;
}

auto get_height_data(const compact_heightfield& chf, const std::uint16_t* poly, std::int32_t npoly, const std::uint16_t* verts, std::int32_t bs, height_patch& hp, std::pmr::vector<std::int32_t>& queue, std::int32_t region) -> void {
  queue.clear();
  std::fill(hp.data.begin(), hp.data.end(), unset_height);

  auto empty = true;

  if (region != 0) {
    for (auto hy = std::int32_t{0}; hy < hp.height; ++hy) {
      const auto y = hp.ymin + hy + bs;

      for (auto hx = std::int32_t{0}; hx < hp.width; ++hx) {
        const auto x = hp.xmin + hx + bs;
        const auto& c = chf.cells[static_cast<std::size_t>(x + y * chf.width)];

        for (auto i = static_cast<std::int32_t>(c.index), ni = static_cast<std::int32_t>(c.index) + static_cast<std::int32_t>(c.count); i < ni; ++i) {
          const auto& s = chf.spans[static_cast<std::size_t>(i)];

          if (s.region_id == static_cast<std::uint16_t>(region)) {
            hp.data[static_cast<std::size_t>(hx + hy * hp.width)] = s.min;
            empty = false;

            auto border = false;

            for (auto dir = std::int32_t{0}; dir < 4; ++dir) {
              if (get_connection(s, dir) != static_cast<std::int32_t>(not_connected)) {
                const auto ax = x + direction_offset_x(dir);
                const auto ay = y + direction_offset_z(dir);
                const auto ai = static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>(ax + ay * chf.width)].index) + get_connection(s, dir);
                const auto& as = chf.spans[static_cast<std::size_t>(ai)];

                if (as.region_id != static_cast<std::uint16_t>(region)) {
                  border = true;
                  break;
                }
              }
            }

            if (border) { push3(queue, x, y, i); }
            break;
          }
        }
      }
    }
  }

  if (empty) {
    seed_array_with_poly_center(chf, poly, npoly, verts, bs, hp, queue);
  }

  auto head = std::int32_t{0};

  while (head * 3 < static_cast<std::int32_t>(queue.size())) {
    const auto cx = queue[static_cast<std::size_t>(head * 3 + 0)];
    const auto cy = queue[static_cast<std::size_t>(head * 3 + 1)];
    const auto ci = queue[static_cast<std::size_t>(head * 3 + 2)];
    ++head;

    if (head >= retract_size) {
      head = 0;
      if (static_cast<std::int32_t>(queue.size()) > retract_size * 3) {
        std::copy(queue.begin() + retract_size * 3, queue.end(), queue.begin());
      }
      queue.resize(queue.size() - static_cast<std::size_t>(retract_size * 3));
    }

    const auto& cs = chf.spans[static_cast<std::size_t>(ci)];

    for (auto dir = std::int32_t{0}; dir < 4; ++dir) {
      if (get_connection(cs, dir) == static_cast<std::int32_t>(not_connected)) { continue; }

      const auto ax = cx + direction_offset_x(dir);
      const auto ay = cy + direction_offset_z(dir);
      const auto hx = ax - hp.xmin - bs;
      const auto hy = ay - hp.ymin - bs;

      if (hx < 0 || hx >= hp.width || hy < 0 || hy >= hp.height) { continue; }
      if (hp.data[static_cast<std::size_t>(hx + hy * hp.width)] != unset_height) { continue; }

      const auto ai = static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>(ax + ay * chf.width)].index) + get_connection(cs, dir);
      const auto& as = chf.spans[static_cast<std::size_t>(ai)];

      hp.data[static_cast<std::size_t>(hx + hy * hp.width)] = as.min;

      push3(queue, ax, ay, ai);
    }
  }
}

[[nodiscard]] auto build_poly_mesh_detail(const poly_mesh& pmesh, const compact_heightfield& chf, std::float_t sample_distance, std::float_t sample_max_error, bake_arena& arena) -> poly_mesh_detail {
  auto detail = poly_mesh_detail{arena.permanent()};

  if (pmesh.num_verts == 0 || pmesh.num_polys == 0) {
    return detail;
  }

  const auto nvp = pmesh.max_verts_per_poly;
  const auto cs = pmesh.cell_size;
  const auto ch = pmesh.cell_height;
  const auto orig = pmesh.bounds.min();
  const auto border_size = pmesh.border_size;
  const auto height_search_radius = std::max(std::int32_t{1}, static_cast<std::int32_t>(std::ceil(pmesh.max_edge_error)));

  auto edges = std::pmr::vector<std::int32_t>{arena.temp()};
  auto tris = std::pmr::vector<std::int32_t>{arena.temp()};
  auto queue = std::pmr::vector<std::int32_t>{arena.temp()};
  auto samples = std::pmr::vector<std::int32_t>{arena.temp()};
  auto verts = std::array<std::float_t, 256u * 3u>{};
  auto poly = std::pmr::vector<std::float_t>(static_cast<std::size_t>(nvp) * 3u, 0.0f, arena.temp());
  auto bounds = std::pmr::vector<std::array<std::int32_t, 4>>(static_cast<std::size_t>(pmesh.num_polys), std::array<std::int32_t, 4>{}, arena.temp());

  auto maxhw = std::int32_t{0};
  auto maxhh = std::int32_t{0};

  for (auto i = std::int32_t{0}; i < pmesh.num_polys; ++i) {
    const auto* p = &pmesh.polys[static_cast<std::size_t>(i) * static_cast<std::size_t>(nvp) * 2u];

    auto xmin = chf.width;
    auto xmax = std::int32_t{0};
    auto ymin = chf.height;
    auto ymax = std::int32_t{0};

    for (auto j = std::int32_t{0}; j < nvp; ++j) {
      if (p[j] == mesh_null_index) { break; }

      const auto* v = &pmesh.verts[static_cast<std::size_t>(p[j]) * 3u];
      xmin = std::min(xmin, static_cast<std::int32_t>(v[0]));
      xmax = std::max(xmax, static_cast<std::int32_t>(v[0]));
      ymin = std::min(ymin, static_cast<std::int32_t>(v[2]));
      ymax = std::max(ymax, static_cast<std::int32_t>(v[2]));
    }

    xmin = std::max(std::int32_t{0}, xmin - 1);
    xmax = std::min(chf.width, xmax + 1);
    ymin = std::max(std::int32_t{0}, ymin - 1);
    ymax = std::min(chf.height, ymax + 1);

    bounds[static_cast<std::size_t>(i)] = std::array<std::int32_t, 4>{xmin, xmax, ymin, ymax};

    if (xmin >= xmax || ymin >= ymax) { continue; }

    maxhw = std::max(maxhw, xmax - xmin);
    maxhh = std::max(maxhh, ymax - ymin);
  }

  auto hp = height_patch{};
  hp.data = std::pmr::vector<std::uint16_t>(static_cast<std::size_t>(std::max(maxhw * maxhh, 1)), std::uint16_t{0}, arena.temp());

  for (auto i = std::int32_t{0}; i < pmesh.num_polys; ++i) {
    const auto* p = &pmesh.polys[static_cast<std::size_t>(i) * static_cast<std::size_t>(nvp) * 2u];

    auto npoly = std::int32_t{0};

    for (auto j = std::int32_t{0}; j < nvp; ++j) {
      if (p[j] == mesh_null_index) { break; }

      const auto* v = &pmesh.verts[static_cast<std::size_t>(p[j]) * 3u];
      poly[static_cast<std::size_t>(j * 3 + 0)] = static_cast<std::float_t>(v[0]) * cs;
      poly[static_cast<std::size_t>(j * 3 + 1)] = static_cast<std::float_t>(v[1]) * ch;
      poly[static_cast<std::size_t>(j * 3 + 2)] = static_cast<std::float_t>(v[2]) * cs;
      ++npoly;
    }

    hp.xmin = bounds[static_cast<std::size_t>(i)][0];
    hp.ymin = bounds[static_cast<std::size_t>(i)][2];
    hp.width = bounds[static_cast<std::size_t>(i)][1] - bounds[static_cast<std::size_t>(i)][0];
    hp.height = bounds[static_cast<std::size_t>(i)][3] - bounds[static_cast<std::size_t>(i)][2];

    get_height_data(chf, p, npoly, pmesh.verts.data(), border_size, hp, queue, static_cast<std::int32_t>(pmesh.regions[static_cast<std::size_t>(i)]));

    auto nverts = std::int32_t{0};

    build_poly_detail(poly.data(), npoly, sample_distance, sample_max_error, height_search_radius, chf, hp, verts.data(), nverts, tris, edges, samples);

    for (auto j = std::int32_t{0}; j < nverts; ++j) {
      verts[static_cast<std::size_t>(j * 3 + 0)] += orig.x();
      verts[static_cast<std::size_t>(j * 3 + 1)] += orig.y() + chf.cell_height;
      verts[static_cast<std::size_t>(j * 3 + 2)] += orig.z();
    }

    const auto vert_base = static_cast<std::uint32_t>(detail.verts.size());
    const auto tri_base = static_cast<std::uint32_t>(detail.tris.size());

    for (auto j = std::int32_t{0}; j < nverts; ++j) {
      detail.verts.push_back(std::array<std::float_t, 3>{
        verts[static_cast<std::size_t>(j * 3 + 0)],
        verts[static_cast<std::size_t>(j * 3 + 1)],
        verts[static_cast<std::size_t>(j * 3 + 2)]
      });
    }

    const auto ntris = static_cast<std::int32_t>(tris.size()) / 4;

    for (auto j = std::int32_t{0}; j < ntris; ++j) {
      const auto* t = &tris[static_cast<std::size_t>(j * 4)];
      detail.tris.push_back(std::array<std::uint32_t, 3>{
        vert_base + static_cast<std::uint32_t>(t[0]),
        vert_base + static_cast<std::uint32_t>(t[1]),
        vert_base + static_cast<std::uint32_t>(t[2])
      });
    }

    detail.meshes.push_back(std::array<std::uint32_t, 4>{vert_base, static_cast<std::uint32_t>(nverts), tri_base, static_cast<std::uint32_t>(ntris)});
  }

  return detail;
}

} // namespace sbx::physics
