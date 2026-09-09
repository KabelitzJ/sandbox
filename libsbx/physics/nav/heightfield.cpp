// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/heightfield.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace sbx::physics {

auto alloc_span(heightfield& hf) -> memory::observer_ptr<span> {
  if (!hf.free_list || !hf.free_list->next) {
    auto& pool = hf.pools.emplace_back();

    auto free_list = hf.free_list;

    for (auto index = span_traits::pool_size; index > 0u; ) {
      --index;

      auto& item = pool.buffer[index];
      item.next = free_list;
      free_list = memory::observer_ptr<span>{&item};
    }

    hf.free_list = free_list;
  }

  auto new_span = hf.free_list;
  hf.free_list = hf.free_list->next;

  return new_span;
}

auto free_span(heightfield& hf, memory::observer_ptr<span> value) -> void {
  if (!value) {
    return;
  }

  value->next = hf.free_list;
  hf.free_list = value;
}

auto add_span(heightfield& hf, std::int32_t x, std::int32_t z, std::uint32_t min, std::uint32_t max, std::uint8_t area, std::int32_t flag_merge_threshold) -> void {
  auto new_span = alloc_span(hf);

  new_span->min = min;
  new_span->max = max;
  new_span->area = area;
  new_span->next = nullptr;

  const auto column_index = x + z * hf.width;

  auto previous_span = memory::observer_ptr<span>{};
  auto current_span = hf.columns[static_cast<std::size_t>(column_index)];

  while (current_span) {
    if (current_span->min > new_span->max) {
      break;
    }

    if (current_span->max < new_span->min) {
      previous_span = current_span;
      current_span = current_span->next;
    } else {
      if (current_span->min < new_span->min) {
        new_span->min = current_span->min;
      }

      if (current_span->max > new_span->max) {
        new_span->max = current_span->max;
      }

      if (std::abs(static_cast<std::int32_t>(new_span->max) - static_cast<std::int32_t>(current_span->max)) <= flag_merge_threshold) {
        new_span->area = std::max(new_span->area, current_span->area);
      }

      auto next = current_span->next;
      free_span(hf, current_span);

      if (previous_span) {
        previous_span->next = next;
      } else {
        hf.columns[static_cast<std::size_t>(column_index)] = next;
      }

      current_span = next;
    }
  }

  if (previous_span) {
    new_span->next = previous_span->next;
    previous_span->next = new_span;
  } else {
    new_span->next = hf.columns[static_cast<std::size_t>(column_index)];
    hf.columns[static_cast<std::size_t>(column_index)] = new_span;
  }
}

[[nodiscard]] auto vector_component(const math::vector3& value, std::int32_t axis) -> std::float_t {
  return axis == 0 ? value.x() : (axis == 1 ? value.y() : value.z());
}

auto divide_polygon(const std::array<math::vector3, 7>& in, std::int32_t in_count, std::array<math::vector3, 7>& out1, std::int32_t& out1_count, std::array<math::vector3, 7>& out2, std::int32_t& out2_count, std::float_t axis_offset, std::int32_t axis) -> void {
  auto delta = std::array<std::float_t, 7>{};

  for (auto i = std::int32_t{0}; i < in_count; ++i) {
    delta[static_cast<std::size_t>(i)] = axis_offset - vector_component(in[static_cast<std::size_t>(i)], axis);
  }

  auto poly1_count = std::int32_t{0};
  auto poly2_count = std::int32_t{0};

  for (auto a = std::int32_t{0}, b = in_count - 1; a < in_count; b = a, ++a) {
    const auto delta_a = delta[static_cast<std::size_t>(a)];
    const auto delta_b = delta[static_cast<std::size_t>(b)];
    const auto same_side = (delta_a >= 0.0f) == (delta_b >= 0.0f);

    if (!same_side) {
      const auto s = delta_b / (delta_b - delta_a);

      out1[static_cast<std::size_t>(poly1_count)] = in[static_cast<std::size_t>(b)] + (in[static_cast<std::size_t>(a)] - in[static_cast<std::size_t>(b)]) * s;
      out2[static_cast<std::size_t>(poly2_count)] = out1[static_cast<std::size_t>(poly1_count)];
      ++poly1_count;
      ++poly2_count;

      if (delta_a > 0.0f) {
        out1[static_cast<std::size_t>(poly1_count)] = in[static_cast<std::size_t>(a)];
        ++poly1_count;
      } else if (delta_a < 0.0f) {
        out2[static_cast<std::size_t>(poly2_count)] = in[static_cast<std::size_t>(a)];
        ++poly2_count;
      }
    } else {
      if (delta_a >= 0.0f) {
        out1[static_cast<std::size_t>(poly1_count)] = in[static_cast<std::size_t>(a)];
        ++poly1_count;

        if (delta_a != 0.0f) {
          continue;
        }
      }

      out2[static_cast<std::size_t>(poly2_count)] = in[static_cast<std::size_t>(a)];
      ++poly2_count;
    }
  }

  out1_count = poly1_count;
  out2_count = poly2_count;
}

[[nodiscard]] auto calc_grid_size(const math::volume& bounds, std::float_t cell_size) -> std::pair<std::int32_t, std::int32_t> {
  const auto width = static_cast<std::int32_t>((bounds.max().x() - bounds.min().x()) / cell_size + 0.5f);
  const auto depth = static_cast<std::int32_t>((bounds.max().z() - bounds.min().z()) / cell_size + 0.5f);

  return {width, depth};
}

[[nodiscard]] auto create_heightfield(std::int32_t width, std::int32_t height, const math::volume& bounds, std::float_t cell_size, std::float_t cell_height, std::pmr::memory_resource* resource) -> heightfield {
  auto hf = heightfield{resource};

  hf.width = width;
  hf.height = height;
  hf.bounds = bounds;
  hf.cell_size = cell_size;
  hf.cell_height = cell_height;
  hf.columns.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

  return hf;
}

[[nodiscard]] auto is_walkable_triangle(const math::vector3& v0, const math::vector3& v1, const math::vector3& v2, std::float_t walkable_slope_angle) -> bool {
  const auto e0 = v1 - v0;
  const auto e1 = v2 - v0;
  const auto normal = math::vector3::normalized(math::vector3::cross(e0, e1));

  const auto walkable_threshold = std::cos(walkable_slope_angle / 180.0f * std::numbers::pi_v<std::float_t>);

  return normal.y() > walkable_threshold;
}

auto rasterize_triangle(heightfield& hf, const math::vector3& v0, const math::vector3& v1, const math::vector3& v2, std::uint8_t area, std::int32_t flag_merge_threshold) -> void {
  auto triangle_bounds = math::volume{};
  triangle_bounds.include(v0);
  triangle_bounds.include(v1);
  triangle_bounds.include(v2);

  if (!triangle_bounds.intersects(hf.bounds)) {
    return;
  }

  const auto width = hf.width;
  const auto height = hf.height;
  const auto by = hf.bounds.max().y() - hf.bounds.min().y();
  const auto inverse_cell_size = 1.0f / hf.cell_size;
  const auto inverse_cell_height = 1.0f / hf.cell_height;

  auto z0 = static_cast<std::int32_t>((triangle_bounds.min().z() - hf.bounds.min().z()) * inverse_cell_size);
  auto z1 = static_cast<std::int32_t>((triangle_bounds.max().z() - hf.bounds.min().z()) * inverse_cell_size);

  z0 = std::clamp(z0, -1, height - 1);
  z1 = std::clamp(z1, 0, height - 1);

  auto in = std::array<math::vector3, 7>{v0, v1, v2};
  auto in_row = std::array<math::vector3, 7>{};
  auto p1 = std::array<math::vector3, 7>{};
  auto p2 = std::array<math::vector3, 7>{};

  auto in_count = std::int32_t{3};
  auto row_count = std::int32_t{0};

  for (auto z = z0; z <= z1; ++z) {
    const auto cell_z = hf.bounds.min().z() + static_cast<std::float_t>(z) * hf.cell_size;

    divide_polygon(in, in_count, in_row, row_count, p1, in_count, cell_z + hf.cell_size, 2);
    std::swap(in, p1);

    if (row_count < 3) {
      continue;
    }

    if (z < 0) {
      continue;
    }

    auto min_x = in_row[0].x();
    auto max_x = in_row[0].x();

    for (auto vert = std::int32_t{1}; vert < row_count; ++vert) {
      min_x = std::min(min_x, in_row[static_cast<std::size_t>(vert)].x());
      max_x = std::max(max_x, in_row[static_cast<std::size_t>(vert)].x());
    }

    auto x0 = static_cast<std::int32_t>((min_x - hf.bounds.min().x()) * inverse_cell_size);
    auto x1 = static_cast<std::int32_t>((max_x - hf.bounds.min().x()) * inverse_cell_size);

    if (x1 < 0 || x0 >= width) {
      continue;
    }

    x0 = std::clamp(x0, -1, width - 1);
    x1 = std::clamp(x1, 0, width - 1);

    auto nv = std::int32_t{0};
    auto nv2 = row_count;

    for (auto x = x0; x <= x1; ++x) {
      const auto cell_x = hf.bounds.min().x() + static_cast<std::float_t>(x) * hf.cell_size;

      divide_polygon(in_row, nv2, p1, nv, p2, nv2, cell_x + hf.cell_size, 0);
      std::swap(in_row, p2);

      if (nv < 3) {
        continue;
      }

      if (x < 0) {
        continue;
      }

      auto span_min = p1[0].y();
      auto span_max = p1[0].y();

      for (auto vert = std::int32_t{1}; vert < nv; ++vert) {
        span_min = std::min(span_min, p1[static_cast<std::size_t>(vert)].y());
        span_max = std::max(span_max, p1[static_cast<std::size_t>(vert)].y());
      }

      span_min -= hf.bounds.min().y();
      span_max -= hf.bounds.min().y();

      if (span_max < 0.0f) {
        continue;
      }

      if (span_min > by) {
        continue;
      }

      span_min = std::max(span_min, 0.0f);
      span_max = std::min(span_max, by);

      const auto span_min_index = static_cast<std::uint32_t>(std::clamp(static_cast<std::int32_t>(std::floor(span_min * inverse_cell_height)), 0, static_cast<std::int32_t>(span_traits::max_height_value)));
      const auto span_max_index = static_cast<std::uint32_t>(std::clamp(static_cast<std::int32_t>(std::ceil(span_max * inverse_cell_height)), static_cast<std::int32_t>(span_min_index) + 1, static_cast<std::int32_t>(span_traits::max_height_value)));

      add_span(hf, x, z, span_min_index, span_max_index, area, flag_merge_threshold);
    }
  }
}

auto rasterize_triangles(heightfield& hf, std::span<const math::vector3> vertices, std::span<const std::uint32_t> indices, std::float_t walkable_slope_angle, std::int32_t flag_merge_threshold) -> void {
  const auto triangle_count = indices.size() / 3u;

  for (auto i = std::size_t{0}; i < triangle_count; ++i) {
    const auto& v0 = vertices[indices[i * 3u + 0u]];
    const auto& v1 = vertices[indices[i * 3u + 1u]];
    const auto& v2 = vertices[indices[i * 3u + 2u]];

    const auto area = is_walkable_triangle(v0, v1, v2, walkable_slope_angle) ? walkable_area : null_area;

    rasterize_triangle(hf, v0, v1, v2, area, flag_merge_threshold);
  }
}

auto filter_low_hanging_walkable_obstacles(heightfield& hf, std::int32_t walkable_climb) -> void {
  for (auto z = std::int32_t{0}; z < hf.height; ++z) {
    for (auto x = std::int32_t{0}; x < hf.width; ++x) {
      auto previous_span = memory::observer_ptr<span>{};
      auto previous_was_walkable = false;
      auto previous_area = null_area;

      for (auto current = hf.columns[static_cast<std::size_t>(x + z * hf.width)]; current; previous_span = current, current = current->next) {
        const auto walkable = current->area != null_area;

        if (!walkable && previous_was_walkable && static_cast<std::int32_t>(current->max) - static_cast<std::int32_t>(previous_span->max) <= walkable_climb) {
          current->area = previous_area;
        }

        previous_was_walkable = walkable;
        previous_area = static_cast<std::uint8_t>(current->area);
      }
    }
  }
}

auto filter_ledge_spans(heightfield& hf, std::int32_t walkable_height, std::int32_t walkable_climb) -> void {
  constexpr auto max_height = std::int32_t{0xffff};

  for (auto z = std::int32_t{0}; z < hf.height; ++z) {
    for (auto x = std::int32_t{0}; x < hf.width; ++x) {
      for (auto current = hf.columns[static_cast<std::size_t>(x + z * hf.width)]; current; current = current->next) {
        if (current->area == null_area) {
          continue;
        }

        const auto floor = static_cast<std::int32_t>(current->max);
        const auto ceiling = current->next ? static_cast<std::int32_t>(current->next->min) : max_height;

        auto lowest_neighbor_floor_difference = max_height;
        auto lowest_traversable_neighbor_floor = floor;
        auto highest_traversable_neighbor_floor = floor;

        for (auto direction = std::int32_t{0}; direction < 4; ++direction) {
          const auto neighbor_x = x + direction_offset_x(direction);
          const auto neighbor_z = z + direction_offset_z(direction);

          if (neighbor_x < 0 || neighbor_z < 0 || neighbor_x >= hf.width || neighbor_z >= hf.height) {
            lowest_neighbor_floor_difference = -walkable_climb - 1;
            break;
          }

          auto neighbor_span = hf.columns[static_cast<std::size_t>(neighbor_x + neighbor_z * hf.width)];
          auto neighbor_ceiling = neighbor_span ? static_cast<std::int32_t>(neighbor_span->min) : max_height;

          if (std::min(ceiling, neighbor_ceiling) - floor >= walkable_height) {
            lowest_neighbor_floor_difference = -walkable_climb - 1;
            break;
          }

          for (; neighbor_span; neighbor_span = neighbor_span->next) {
            const auto neighbor_floor = static_cast<std::int32_t>(neighbor_span->max);
            neighbor_ceiling = neighbor_span->next ? static_cast<std::int32_t>(neighbor_span->next->min) : max_height;

            if (std::min(ceiling, neighbor_ceiling) - std::max(floor, neighbor_floor) < walkable_height) {
              continue;
            }

            const auto neighbor_floor_difference = neighbor_floor - floor;
            lowest_neighbor_floor_difference = std::min(lowest_neighbor_floor_difference, neighbor_floor_difference);

            if (std::abs(neighbor_floor_difference) <= walkable_climb) {
              lowest_traversable_neighbor_floor = std::min(lowest_traversable_neighbor_floor, neighbor_floor);
              highest_traversable_neighbor_floor = std::max(highest_traversable_neighbor_floor, neighbor_floor);
            } else if (neighbor_floor_difference < -walkable_climb) {
              break;
            }
          }
        }

        if (lowest_neighbor_floor_difference < -walkable_climb) {
          current->area = null_area;
        } else if (highest_traversable_neighbor_floor - lowest_traversable_neighbor_floor > walkable_climb) {
          current->area = null_area;
        }
      }
    }
  }
}

auto filter_walkable_low_height_spans(heightfield& hf, std::int32_t walkable_height) -> void {
  constexpr auto max_height = std::int32_t{0xffff};

  for (auto z = std::int32_t{0}; z < hf.height; ++z) {
    for (auto x = std::int32_t{0}; x < hf.width; ++x) {
      for (auto current = hf.columns[static_cast<std::size_t>(x + z * hf.width)]; current; current = current->next) {
        const auto floor = static_cast<std::int32_t>(current->max);
        const auto ceiling = current->next ? static_cast<std::int32_t>(current->next->min) : max_height;

        if (ceiling - floor < walkable_height) {
          current->area = null_area;
        }
      }
    }
  }
}

} // namespace sbx::physics
