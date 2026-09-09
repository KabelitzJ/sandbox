// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/contour.hpp>

#include <algorithm>
#include <array>
#include <cmath>

#include <libsbx/memory/observer_ptr.hpp>

namespace sbx::physics {

[[nodiscard]] auto get_corner_height(std::int32_t x, std::int32_t z, std::int32_t i, std::int32_t direction, const compact_heightfield& chf, bool& is_border_vertex) -> std::int32_t {
  const auto& span = chf.spans[static_cast<std::size_t>(i)];
  auto ch = static_cast<std::int32_t>(span.min);
  const auto dir_p = (direction + 1) & 0x3;

  auto regs = std::array<std::uint32_t, 4>{0u, 0u, 0u, 0u};

  regs[0] = static_cast<std::uint32_t>(chf.spans[static_cast<std::size_t>(i)].region_id) | (static_cast<std::uint32_t>(chf.areas[static_cast<std::size_t>(i)]) << 16);

  if (get_connection(span, direction) != static_cast<std::int32_t>(not_connected)) {
    const auto ax = x + direction_offset_x(direction);
    const auto az = z + direction_offset_z(direction);
    const auto ai = static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>(ax + az * chf.width)].index) + get_connection(span, direction);
    const auto& a_span = chf.spans[static_cast<std::size_t>(ai)];

    ch = std::max(ch, static_cast<std::int32_t>(a_span.min));
    regs[1] = static_cast<std::uint32_t>(chf.spans[static_cast<std::size_t>(ai)].region_id) | (static_cast<std::uint32_t>(chf.areas[static_cast<std::size_t>(ai)]) << 16);

    if (get_connection(a_span, dir_p) != static_cast<std::int32_t>(not_connected)) {
      const auto ax2 = ax + direction_offset_x(dir_p);
      const auto az2 = az + direction_offset_z(dir_p);
      const auto ai2 = static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>(ax2 + az2 * chf.width)].index) + get_connection(a_span, dir_p);
      const auto& a_span2 = chf.spans[static_cast<std::size_t>(ai2)];

      ch = std::max(ch, static_cast<std::int32_t>(a_span2.min));
      regs[2] = static_cast<std::uint32_t>(chf.spans[static_cast<std::size_t>(ai2)].region_id) | (static_cast<std::uint32_t>(chf.areas[static_cast<std::size_t>(ai2)]) << 16);
    }
  }

  if (get_connection(span, dir_p) != static_cast<std::int32_t>(not_connected)) {
    const auto ax = x + direction_offset_x(dir_p);
    const auto az = z + direction_offset_z(dir_p);
    const auto ai = static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>(ax + az * chf.width)].index) + get_connection(span, dir_p);
    const auto& a_span = chf.spans[static_cast<std::size_t>(ai)];

    ch = std::max(ch, static_cast<std::int32_t>(a_span.min));
    regs[3] = static_cast<std::uint32_t>(chf.spans[static_cast<std::size_t>(ai)].region_id) | (static_cast<std::uint32_t>(chf.areas[static_cast<std::size_t>(ai)]) << 16);

    if (get_connection(a_span, direction) != static_cast<std::int32_t>(not_connected)) {
      const auto ax2 = ax + direction_offset_x(direction);
      const auto az2 = az + direction_offset_z(direction);
      const auto ai2 = static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>(ax2 + az2 * chf.width)].index) + get_connection(a_span, direction);
      const auto& a_span2 = chf.spans[static_cast<std::size_t>(ai2)];

      ch = std::max(ch, static_cast<std::int32_t>(a_span2.min));
      regs[2] = static_cast<std::uint32_t>(chf.spans[static_cast<std::size_t>(ai2)].region_id) | (static_cast<std::uint32_t>(chf.areas[static_cast<std::size_t>(ai2)]) << 16);
    }
  }

  for (auto j = std::int32_t{0}; j < 4; ++j) {
    const auto a = static_cast<std::size_t>(j);
    const auto b = static_cast<std::size_t>((j + 1) & 0x3);
    const auto c = static_cast<std::size_t>((j + 2) & 0x3);
    const auto d = static_cast<std::size_t>((j + 3) & 0x3);

    const auto two_same_exts = (regs[a] & regs[b] & border_region) != 0 && regs[a] == regs[b];
    const auto two_ints = ((regs[c] | regs[d]) & border_region) == 0;
    const auto ints_same_area = (regs[c] >> 16) == (regs[d] >> 16);
    const auto no_zeros = regs[0] != 0 && regs[1] != 0 && regs[2] != 0 && regs[3] != 0;

    if (two_same_exts && two_ints && ints_same_area && no_zeros) {
      is_border_vertex = true;
      break;
    }
  }

  return ch;
}

auto walk_contour(std::int32_t x, std::int32_t z, std::int32_t i, const compact_heightfield& chf, std::pmr::vector<std::uint8_t>& flags, std::pmr::vector<contour_vertex>& points) -> void {
  auto direction = std::uint8_t{0};

  while ((flags[static_cast<std::size_t>(i)] & static_cast<std::uint8_t>(1u << direction)) == 0) {
    ++direction;
  }

  const auto start_dir = direction;
  const auto start_i = i;

  const auto area = chf.areas[static_cast<std::size_t>(i)];

  auto iter = std::int32_t{0};

  while (++iter < 40000) {
    if (flags[static_cast<std::size_t>(i)] & static_cast<std::uint8_t>(1u << direction)) {
      auto is_border_vertex = false;
      auto is_area_border = false;
      auto px = x;
      auto py = get_corner_height(x, z, i, direction, chf, is_border_vertex);
      auto pz = z;

      switch (direction) {
        case 0: ++pz; break;
        case 1: ++px; ++pz; break;
        case 2: ++px; break;
        default: break;
      }

      auto r = std::int32_t{0};
      const auto& span = chf.spans[static_cast<std::size_t>(i)];

      if (get_connection(span, direction) != static_cast<std::int32_t>(not_connected)) {
        const auto ax = x + direction_offset_x(direction);
        const auto az = z + direction_offset_z(direction);
        const auto ai = static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>(ax + az * chf.width)].index) + get_connection(span, direction);

        r = static_cast<std::int32_t>(chf.spans[static_cast<std::size_t>(ai)].region_id);

        if (area != chf.areas[static_cast<std::size_t>(ai)]) {
          is_area_border = true;
        }
      }

      if (is_border_vertex) {
        r |= border_vertex;
      }

      if (is_area_border) {
        r |= area_border;
      }

      points.push_back(contour_vertex{px, py, pz, r});

      flags[static_cast<std::size_t>(i)] &= static_cast<std::uint8_t>(~(1u << direction));
      direction = static_cast<std::uint8_t>((direction + 1u) & 0x3u);
    } else {
      auto ni = std::int32_t{-1};
      const auto nx = x + direction_offset_x(direction);
      const auto nz = z + direction_offset_z(direction);
      const auto& span = chf.spans[static_cast<std::size_t>(i)];

      if (get_connection(span, direction) != static_cast<std::int32_t>(not_connected)) {
        const auto& neighbor_cell = chf.cells[static_cast<std::size_t>(nx + nz * chf.width)];
        ni = static_cast<std::int32_t>(neighbor_cell.index) + get_connection(span, direction);
      }

      if (ni == -1) {
        return;
      }

      x = nx;
      z = nz;
      i = ni;
      direction = static_cast<std::uint8_t>((direction + 3u) & 0x3u);
    }

    if (start_i == i && start_dir == direction) {
      break;
    }
  }
}

[[nodiscard]] auto distance_point_to_segment(std::int32_t x, std::int32_t z, std::int32_t px, std::int32_t pz, std::int32_t qx, std::int32_t qz) -> std::float_t {
  const auto pqx = static_cast<std::float_t>(qx - px);
  const auto pqz = static_cast<std::float_t>(qz - pz);
  auto dx = static_cast<std::float_t>(x - px);
  auto dz = static_cast<std::float_t>(z - pz);
  const auto d = pqx * pqx + pqz * pqz;
  auto t = pqx * dx + pqz * dz;

  if (d > 0.0f) {
    t /= d;
  }

  t = std::clamp(t, 0.0f, 1.0f);

  dx = static_cast<std::float_t>(px) + t * pqx - static_cast<std::float_t>(x);
  dz = static_cast<std::float_t>(pz) + t * pqz - static_cast<std::float_t>(z);

  return dx * dx + dz * dz;
}

struct simplify_point {
  std::int32_t x{0};
  std::int32_t y{0};
  std::int32_t z{0};
  std::int32_t index{0};
}; // struct simplify_point

auto simplify_contour(const std::pmr::vector<contour_vertex>& points, std::pmr::vector<simplify_point>& simplified, std::float_t max_error, std::int32_t max_edge_length, std::int32_t build_flags) -> void {
  auto has_connections = false;

  for (const auto& point : points) {
    if ((point.flags & contour_region_mask) != 0) {
      has_connections = true;
      break;
    }
  }

  const auto pn = static_cast<std::int32_t>(points.size());

  if (has_connections) {
    for (auto i = std::int32_t{0}; i < pn; ++i) {
      const auto ii = (i + 1) % pn;

      const auto different_regs = (points[static_cast<std::size_t>(i)].flags & contour_region_mask) != (points[static_cast<std::size_t>(ii)].flags & contour_region_mask);
      const auto area_borders = (points[static_cast<std::size_t>(i)].flags & area_border) != (points[static_cast<std::size_t>(ii)].flags & area_border);

      if (different_regs || area_borders) {
        const auto& p = points[static_cast<std::size_t>(i)];
        simplified.push_back(simplify_point{p.x, p.y, p.z, i});
      }
    }
  }

  if (simplified.empty()) {
    auto ll = points[0];
    auto lli = std::int32_t{0};
    auto ur = points[0];
    auto uri = std::int32_t{0};

    for (auto i = std::size_t{0}; i < points.size(); ++i) {
      const auto& p = points[i];

      if (p.x < ll.x || (p.x == ll.x && p.z < ll.z)) {
        ll = p;
        lli = static_cast<std::int32_t>(i);
      }

      if (p.x > ur.x || (p.x == ur.x && p.z > ur.z)) {
        ur = p;
        uri = static_cast<std::int32_t>(i);
      }
    }

    simplified.push_back(simplify_point{ll.x, ll.y, ll.z, lli});
    simplified.push_back(simplify_point{ur.x, ur.y, ur.z, uri});
  }

  for (auto i = std::int32_t{0}; i < static_cast<std::int32_t>(simplified.size()); ) {
    const auto ii = (i + 1) % static_cast<std::int32_t>(simplified.size());

    auto ax = simplified[static_cast<std::size_t>(i)].x;
    auto az = simplified[static_cast<std::size_t>(i)].z;
    const auto ai = simplified[static_cast<std::size_t>(i)].index;

    auto bx = simplified[static_cast<std::size_t>(ii)].x;
    auto bz = simplified[static_cast<std::size_t>(ii)].z;
    const auto bi = simplified[static_cast<std::size_t>(ii)].index;

    auto max_d = 0.0f;
    auto max_i = std::int32_t{-1};

    std::int32_t ci{};
    std::int32_t cinc{};
    std::int32_t end_i{};

    if (bx > ax || (bx == ax && bz > az)) {
      cinc = 1;
      ci = (ai + cinc) % pn;
      end_i = bi;
    } else {
      cinc = pn - 1;
      ci = (bi + cinc) % pn;
      end_i = ai;
      std::swap(ax, bx);
      std::swap(az, bz);
    }

    if ((points[static_cast<std::size_t>(ci)].flags & contour_region_mask) == 0 || (points[static_cast<std::size_t>(ci)].flags & area_border)) {
      while (ci != end_i) {
        const auto d = distance_point_to_segment(points[static_cast<std::size_t>(ci)].x, points[static_cast<std::size_t>(ci)].z, ax, az, bx, bz);

        if (d > max_d) {
          max_d = d;
          max_i = ci;
        }

        ci = (ci + cinc) % pn;
      }
    }

    if (max_i != -1 && max_d > (max_error * max_error)) {
      const auto& p = points[static_cast<std::size_t>(max_i)];
      simplified.insert(simplified.begin() + i + 1, simplify_point{p.x, p.y, p.z, max_i});
    } else {
      ++i;
    }
  }

  if (max_edge_length > 0 && (build_flags & (tess_wall_edges | tess_area_edges)) != 0) {
    for (auto i = std::int32_t{0}; i < static_cast<std::int32_t>(simplified.size()); ) {
      const auto ii = (i + 1) % static_cast<std::int32_t>(simplified.size());

      const auto ax = simplified[static_cast<std::size_t>(i)].x;
      const auto az = simplified[static_cast<std::size_t>(i)].z;
      const auto ai = simplified[static_cast<std::size_t>(i)].index;

      const auto bx = simplified[static_cast<std::size_t>(ii)].x;
      const auto bz = simplified[static_cast<std::size_t>(ii)].z;
      const auto bi = simplified[static_cast<std::size_t>(ii)].index;

      auto max_i = std::int32_t{-1};
      const auto ci = (ai + 1) % pn;

      auto tess = false;

      if ((build_flags & tess_wall_edges) && (points[static_cast<std::size_t>(ci)].flags & contour_region_mask) == 0) {
        tess = true;
      }

      if ((build_flags & tess_area_edges) && (points[static_cast<std::size_t>(ci)].flags & area_border)) {
        tess = true;
      }

      if (tess) {
        const auto dx = bx - ax;
        const auto dz = bz - az;

        if (dx * dx + dz * dz > max_edge_length * max_edge_length) {
          const auto n = bi < ai ? (bi + pn - ai) : (bi - ai);

          if (n > 1) {
            if (bx > ax || (bx == ax && bz > az)) {
              max_i = (ai + n / 2) % pn;
            } else {
              max_i = (ai + (n + 1) / 2) % pn;
            }
          }
        }
      }

      if (max_i != -1) {
        const auto& p = points[static_cast<std::size_t>(max_i)];
        simplified.insert(simplified.begin() + i + 1, simplify_point{p.x, p.y, p.z, max_i});
      } else {
        ++i;
      }
    }
  }

  for (auto& point : simplified) {
    const auto ai = (point.index + 1) % pn;
    const auto bi = point.index;

    point.index = (points[static_cast<std::size_t>(ai)].flags & (contour_region_mask | area_border)) | (points[static_cast<std::size_t>(bi)].flags & border_vertex);
  }
}

auto remove_degenerate_segments(std::pmr::vector<simplify_point>& simplified) -> void {
  auto count = static_cast<std::int32_t>(simplified.size());

  for (auto i = std::int32_t{0}; i < count; ++i) {
    const auto ni = (i + 1) % count;

    if (simplified[static_cast<std::size_t>(i)].x == simplified[static_cast<std::size_t>(ni)].x && simplified[static_cast<std::size_t>(i)].z == simplified[static_cast<std::size_t>(ni)].z) {
      simplified.erase(simplified.begin() + i);
      --count;
    }
  }
}

[[nodiscard]] auto calc_area_of_polygon_2d(const std::pmr::vector<contour_vertex>& verts) -> std::int32_t {
  auto area = std::int32_t{0};
  const auto n = static_cast<std::int32_t>(verts.size());

  for (auto i = std::int32_t{0}, j = n - 1; i < n; j = i++) {
    const auto& vi = verts[static_cast<std::size_t>(i)];
    const auto& vj = verts[static_cast<std::size_t>(j)];
    area += vi.x * vj.z - vj.x * vi.z;
  }

  return (area + 1) / 2;
}

[[nodiscard]] auto prev_index(std::int32_t i, std::int32_t n) -> std::int32_t {
  return i - 1 >= 0 ? i - 1 : n - 1;
}

[[nodiscard]] auto next_index(std::int32_t i, std::int32_t n) -> std::int32_t {
  return i + 1 < n ? i + 1 : 0;
}

[[nodiscard]] auto area2(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> std::int32_t {
  return (b.x - a.x) * (c.z - a.z) - (c.x - a.x) * (b.z - a.z);
}

[[nodiscard]] auto is_left(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> bool {
  return area2(a, b, c) < 0;
}

[[nodiscard]] auto is_left_on(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> bool {
  return area2(a, b, c) <= 0;
}

[[nodiscard]] auto is_collinear(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> bool {
  return area2(a, b, c) == 0;
}

[[nodiscard]] auto intersect_prop(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c, const contour_vertex& d) -> bool {
  if (is_collinear(a, b, c) || is_collinear(a, b, d) || is_collinear(c, d, a) || is_collinear(c, d, b)) {
    return false;
  }

  return (is_left(a, b, c) != is_left(a, b, d)) && (is_left(c, d, a) != is_left(c, d, b));
}

[[nodiscard]] auto is_between(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c) -> bool {
  if (!is_collinear(a, b, c)) {
    return false;
  }

  if (a.x != b.x) {
    return ((a.x <= c.x) && (c.x <= b.x)) || ((a.x >= c.x) && (c.x >= b.x));
  }

  return ((a.z <= c.z) && (c.z <= b.z)) || ((a.z >= c.z) && (c.z >= b.z));
}

[[nodiscard]] auto segments_intersect(const contour_vertex& a, const contour_vertex& b, const contour_vertex& c, const contour_vertex& d) -> bool {
  if (intersect_prop(a, b, c, d)) {
    return true;
  }

  return is_between(a, b, c) || is_between(a, b, d) || is_between(c, d, a) || is_between(c, d, b);
}

[[nodiscard]] auto vertices_equal(const contour_vertex& a, const contour_vertex& b) -> bool {
  return a.x == b.x && a.z == b.z;
}

[[nodiscard]] auto intersects_contour_segment(const contour_vertex& d0, const contour_vertex& d1, std::int32_t i, const std::pmr::vector<contour_vertex>& verts) -> bool {
  const auto n = static_cast<std::int32_t>(verts.size());

  for (auto k = std::int32_t{0}; k < n; ++k) {
    const auto k1 = next_index(k, n);

    if (i == k || i == k1) {
      continue;
    }

    const auto& p0 = verts[static_cast<std::size_t>(k)];
    const auto& p1 = verts[static_cast<std::size_t>(k1)];

    if (vertices_equal(d0, p0) || vertices_equal(d1, p0) || vertices_equal(d0, p1) || vertices_equal(d1, p1)) {
      continue;
    }

    if (segments_intersect(d0, d1, p0, p1)) {
      return true;
    }
  }

  return false;
}

[[nodiscard]] auto in_cone(std::int32_t i, const std::pmr::vector<contour_vertex>& verts, const contour_vertex& pj) -> bool {
  const auto n = static_cast<std::int32_t>(verts.size());
  const auto& pi = verts[static_cast<std::size_t>(i)];
  const auto& pi1 = verts[static_cast<std::size_t>(next_index(i, n))];
  const auto& pin1 = verts[static_cast<std::size_t>(prev_index(i, n))];

  if (is_left_on(pin1, pi, pi1)) {
    return is_left(pi, pj, pin1) && is_left(pj, pi, pi1);
  }

  return !(is_left_on(pi, pj, pi1) && is_left_on(pj, pi, pin1));
}

auto merge_contours(contour& a, contour& b, std::int32_t ia, std::int32_t ib) -> void {
  auto merged = std::pmr::vector<contour_vertex>{a.verts.get_allocator()};

  const auto a_count = static_cast<std::int32_t>(a.verts.size());
  const auto b_count = static_cast<std::int32_t>(b.verts.size());

  merged.reserve(static_cast<std::size_t>(a_count + b_count) + 2u);

  for (auto i = std::int32_t{0}; i <= a_count; ++i) {
    merged.push_back(a.verts[static_cast<std::size_t>((ia + i) % a_count)]);
  }

  for (auto i = std::int32_t{0}; i <= b_count; ++i) {
    merged.push_back(b.verts[static_cast<std::size_t>((ib + i) % b_count)]);
  }

  a.verts = std::move(merged);
  b.verts.clear();
}

auto find_leftmost_vertex(const contour& value, std::int32_t& min_x, std::int32_t& min_z, std::int32_t& leftmost) -> void {
  min_x = value.verts[0].x;
  min_z = value.verts[0].z;
  leftmost = 0;

  for (auto i = std::size_t{1}; i < value.verts.size(); ++i) {
    const auto x = value.verts[i].x;
    const auto z = value.verts[i].z;

    if (x < min_x || (x == min_x && z < min_z)) {
      min_x = x;
      min_z = z;
      leftmost = static_cast<std::int32_t>(i);
    }
  }
}

struct potential_diagonal {
  std::int32_t vertex{0};
  std::int32_t distance{0};
}; // struct potential_diagonal

auto merge_region_holes(contour& outline, std::pmr::vector<memory::observer_ptr<contour>>& holes, std::pmr::memory_resource* temp_resource) -> void {
  struct hole_info {
    memory::observer_ptr<contour> value;
    std::int32_t min_x{0};
    std::int32_t min_z{0};
    std::int32_t leftmost{0};
  };

  auto infos = std::pmr::vector<hole_info>{temp_resource};
  infos.reserve(holes.size());

  for (auto hole : holes) {
    auto info = hole_info{hole, 0, 0, 0};
    find_leftmost_vertex(*hole, info.min_x, info.min_z, info.leftmost);
    infos.push_back(info);
  }

  std::sort(infos.begin(), infos.end(), [](const hole_info& a, const hole_info& b) {
    if (a.min_x == b.min_x) {
      return a.min_z < b.min_z;
    }

    return a.min_x < b.min_x;
  });

  for (auto& info : infos) {
    auto hole = info.value;

    auto index = std::int32_t{-1};
    auto best_vertex = info.leftmost;

    for (auto iter = std::size_t{0}; iter < hole->verts.size(); ++iter) {
      const auto& corner = hole->verts[static_cast<std::size_t>(best_vertex)];

      auto diagonals = std::pmr::vector<potential_diagonal>{temp_resource};

      for (auto j = std::int32_t{0}; j < static_cast<std::int32_t>(outline.verts.size()); ++j) {
        if (in_cone(j, outline.verts, corner)) {
          const auto dx = outline.verts[static_cast<std::size_t>(j)].x - corner.x;
          const auto dz = outline.verts[static_cast<std::size_t>(j)].z - corner.z;
          diagonals.push_back(potential_diagonal{j, dx * dx + dz * dz});
        }
      }

      std::sort(diagonals.begin(), diagonals.end(), [](const potential_diagonal& a, const potential_diagonal& b) {
        return a.distance < b.distance;
      });

      index = -1;

      for (const auto& diagonal : diagonals) {
        const auto& pt = outline.verts[static_cast<std::size_t>(diagonal.vertex)];

        auto intersects = intersects_contour_segment(pt, corner, diagonal.vertex, outline.verts);

        for (const auto& other : infos) {
          if (intersects) {
            break;
          }

          intersects = intersects_contour_segment(pt, corner, -1, other.value->verts);
        }

        if (!intersects) {
          index = diagonal.vertex;
          break;
        }
      }

      if (index != -1) {
        break;
      }

      best_vertex = static_cast<std::int32_t>((static_cast<std::size_t>(best_vertex) + 1u) % hole->verts.size());
    }

    if (index == -1) {
      continue;
    }

    merge_contours(outline, *hole, index, best_vertex);
  }
}

[[nodiscard]] auto build_contours(const compact_heightfield& chf, std::float_t max_error, std::int32_t max_edge_length, bake_arena& arena, std::int32_t build_flags) -> contour_set {
  auto cset = contour_set{arena.permanent()};

  const auto width = chf.width;
  const auto height = chf.height;
  const auto border_size = chf.border_size;

  cset.bounds = chf.bounds;

  if (border_size > 0) {
    const auto pad = static_cast<std::float_t>(border_size) * chf.cell_size;
    cset.bounds = math::volume{cset.bounds.min() + math::vector3{pad, 0.0f, pad}, cset.bounds.max() - math::vector3{pad, 0.0f, pad}};
  }

  cset.cell_size = chf.cell_size;
  cset.cell_height = chf.cell_height;
  cset.width = chf.width - chf.border_size * 2;
  cset.height = chf.height - chf.border_size * 2;
  cset.border_size = chf.border_size;
  cset.max_error = max_error;

  auto flags = std::pmr::vector<std::uint8_t>(chf.spans.size(), std::uint8_t{0}, arena.temp());

  for (auto z = std::int32_t{0}; z < height; ++z) {
    for (auto x = std::int32_t{0}; x < width; ++x) {
      const auto& cell = chf.cells[static_cast<std::size_t>(x + z * width)];

      for (auto i = static_cast<std::int32_t>(cell.index), ni = static_cast<std::int32_t>(cell.index + cell.count); i < ni; ++i) {
        auto res = std::uint32_t{0};
        const auto& span = chf.spans[static_cast<std::size_t>(i)];

        if (!span.region_id || (span.region_id & border_region)) {
          flags[static_cast<std::size_t>(i)] = 0;
          continue;
        }

        for (auto direction = std::int32_t{0}; direction < 4; ++direction) {
          auto r = std::uint16_t{0};

          if (get_connection(span, direction) != static_cast<std::int32_t>(not_connected)) {
            const auto ax = x + direction_offset_x(direction);
            const auto az = z + direction_offset_z(direction);
            const auto ai = static_cast<std::int32_t>(chf.cells[static_cast<std::size_t>(ax + az * width)].index) + get_connection(span, direction);
            r = chf.spans[static_cast<std::size_t>(ai)].region_id;
          }

          if (r == span.region_id) {
            res |= (1u << direction);
          }
        }

        flags[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(res ^ 0xfu);
      }
    }
  }

  auto raw_points = std::pmr::vector<contour_vertex>{arena.temp()};
  auto simplified_points = std::pmr::vector<simplify_point>{arena.temp()};

  for (auto z = std::int32_t{0}; z < height; ++z) {
    for (auto x = std::int32_t{0}; x < width; ++x) {
      const auto& cell = chf.cells[static_cast<std::size_t>(x + z * width)];

      for (auto i = static_cast<std::int32_t>(cell.index), ni = static_cast<std::int32_t>(cell.index + cell.count); i < ni; ++i) {
        if (flags[static_cast<std::size_t>(i)] == 0 || flags[static_cast<std::size_t>(i)] == 0xfu) {
          flags[static_cast<std::size_t>(i)] = 0;
          continue;
        }

        const auto region = chf.spans[static_cast<std::size_t>(i)].region_id;

        if (!region || (region & border_region)) {
          continue;
        }

        const auto area = chf.areas[static_cast<std::size_t>(i)];

        raw_points.clear();
        simplified_points.clear();

        walk_contour(x, z, i, chf, flags, raw_points);
        simplify_contour(raw_points, simplified_points, max_error, max_edge_length, build_flags);
        remove_degenerate_segments(simplified_points);

        if (simplified_points.size() >= 3u) {
          auto value = contour{arena.permanent()};
          value.region_id = region;
          value.area = area;

          value.verts.reserve(simplified_points.size());

          for (const auto& point : simplified_points) {
            value.verts.push_back(contour_vertex{point.x, point.y, point.z, point.index});
          }

          if (border_size > 0) {
            for (auto& vertex : value.verts) {
              vertex.x -= border_size;
              vertex.z -= border_size;
            }
          }

          value.raw_verts = raw_points;

          if (border_size > 0) {
            for (auto& vertex : value.raw_verts) {
              vertex.x -= border_size;
              vertex.z -= border_size;
            }
          }

          cset.contours.push_back(std::move(value));
        }
      }
    }
  }

  if (!cset.contours.empty()) {
    auto winding = std::pmr::vector<std::int32_t>(cset.contours.size(), arena.temp());
    auto hole_count = std::int32_t{0};

    for (auto i = std::size_t{0}; i < cset.contours.size(); ++i) {
      winding[i] = calc_area_of_polygon_2d(cset.contours[i].verts) < 0 ? -1 : 1;

      if (winding[i] < 0) {
        ++hole_count;
      }
    }

    if (hole_count > 0) {
      const auto region_count = static_cast<std::size_t>(chf.max_regions) + 1u;

      auto outlines = std::pmr::vector<memory::observer_ptr<contour>>(region_count, arena.temp());
      auto holes = std::pmr::vector<std::pmr::vector<memory::observer_ptr<contour>>>(region_count, arena.temp());

      for (auto i = std::size_t{0}; i < cset.contours.size(); ++i) {
        auto& value = cset.contours[i];

        if (winding[i] > 0) {
          outlines[value.region_id] = memory::observer_ptr<contour>{&value};
        } else {
          holes[value.region_id].push_back(memory::observer_ptr<contour>{&value});
        }
      }

      for (auto i = std::size_t{0}; i < region_count; ++i) {
        if (holes[i].empty()) {
          continue;
        }

        if (outlines[i]) {
          merge_region_holes(*outlines[i], holes[i], arena.temp());
        }
      }
    }
  }

  return cset;
}

} // namespace sbx::physics
