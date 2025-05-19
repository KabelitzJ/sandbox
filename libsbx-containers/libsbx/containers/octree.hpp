#ifndef LIBSBX_CONTAINERS_OCTREE_HPP_
#define LIBSBX_CONTAINERS_OCTREE_HPP_

#include <range/v3/all.hpp>

#include <libsbx/utility/enum.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/uuid.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/containers/static_vector.hpp>

namespace sbx::containers {

template<typename Type, std::size_t Threshold = 16u, std::size_t Depth = 8u>
class octree {

  inline static constexpr auto threshold = Threshold;
  inline static constexpr auto depth = Depth;

  struct node {
    
    using id = std::uint32_t;

    struct value_type {
      Type value;
      math::volume bounds;
    }; // struct value_type

    inline constexpr static auto null = static_cast<id>(-1);

    node()
    : children{null, null, null, null, null, null, null, null} { }

    auto is_leaf() const noexcept -> bool {
      return children[0u] == null;
    }

    auto child_at(const std::size_t index) const noexcept -> id {
      return children[index];
    }

    auto push_back(const value_type& value) noexcept -> void {
      values.push_back(value);
    }

    auto push_back(value_type&& value) noexcept -> void {
      values.push_back(std::move(value));
    }

    static auto find_volume(const math::volume& outer, const math::volume& inner) noexcept -> std::optional<std::uint32_t> {
      auto center = outer.center();

      if (!outer.contains(inner)) {
        return std::nullopt;
      }

      if (inner.max().x() <= center.x() && inner.max().y() <= center.y() && inner.max().z() <= center.z()) {
        return 0u;
      }

      if (inner.min().x() >= center.x() && inner.max().y() <= center.y() && inner.max().z() <= center.z()) {
        return 1u;
      }

      if (inner.max().x() <= center.x() && inner.min().y() >= center.y() && inner.max().z() <= center.z()) {
        return 2u;
      }

      if (inner.min().x() >= center.x() && inner.min().y() >= center.y() && inner.max().z() <= center.z()) {
        return 3u;
      }

      if (inner.max().x() <= center.x() && inner.max().y() <= center.y() && inner.min().z() >= center.z()) {
        return 4u;
      }

      if (inner.min().x() >= center.x() && inner.max().y() <= center.y() && inner.min().z() >= center.z()) {
        return 5u;
      }

      if (inner.max().x() <= center.x() && inner.min().y() >= center.y() && inner.min().z() >= center.z()) {
        return 6u;
      }

      if (inner.min().x() >= center.x() && inner.min().y() >= center.y() && inner.min().z() >= center.z()) {
        return 7u;
      }

      return std::nullopt;
    }

    static auto child_bounds(const math::volume& outer, const std::uint32_t index) -> math::volume {
      const auto min = outer.min();
      const auto max = outer.max();
      const auto center = (min + max) * 0.5f;

      switch (index) {
        case 0: return math::volume{min, center};
        case 1: return math::volume{math::vector3{center.x(), min.y(), min.z()}, math::vector3{max.x(), center.y(), center.z()}};
        case 2: return math::volume{math::vector3{min.x(), center.y(), min.z()}, math::vector3{center.x(), max.y(), center.z()}};
        case 3: return math::volume{math::vector3{center.x(), center.y(), min.z()}, math::vector3{max.x(), max.y(), center.z()}};
        case 4: return math::volume{math::vector3{min.x(), min.y(), center.z()}, math::vector3{center.x(), center.y(), max.z()}};
        case 5: return math::volume{math::vector3{center.x(), min.y(), center.z()}, math::vector3{max.x(), center.y(), max.z()}};
        case 6: return math::volume{math::vector3{min.x(), center.y(), center.z()}, math::vector3{center.x(), max.y(), max.z()}};
        case 7: return math::volume{center, max};
      }

      throw std::runtime_error("Invalid index");
    }

    std::vector<value_type> values;
    std::array<id, 8u> children;
  
  }; // struct node

public:

  struct intersection {
    Type first;
    Type second;
  }; // struct intersection

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;

  octree(const math::volume& bounds) noexcept
  : _bounds{bounds},
    _root{0u} {
    _nodes.push_back(node{});
  }

  auto insert(const value_type& value, const math::volume& bounds) noexcept -> void {
    _insert(_root, _bounds, value, bounds, 0u);
  }

  auto intersections() -> std::vector<intersection> {
    auto intersections = std::vector<intersection>{};

    _intersections(_root, _bounds, intersections);

    return intersections;
  }

  auto clear() -> void {
    _nodes.clear();
  }

private:

  auto _insert(const node::id node_id, const math::volume& bounds, const value_type& value, const math::volume& value_bounds, const std::size_t current_depth) noexcept -> void {
    if (!bounds.contains(value_bounds)) {
      return;
    }

    if (_nodes[node_id].is_leaf()) {
      if (_nodes[node_id].values.size() < threshold || current_depth >= depth) {
        _nodes[node_id].push_back({value, value_bounds});
      } else {
        _split(node_id, bounds);
        _insert(node_id, bounds, value, value_bounds, current_depth);
      }
    } else {
      const auto quadrant = node::find_volume(bounds, value_bounds);

      if (quadrant) {
        _insert(_nodes[node_id].child_at(*quadrant), node::child_bounds(bounds, *quadrant), value, value_bounds, current_depth + 1u);
      } else {
        _nodes[node_id].values.push_back({value, value_bounds});
      }
    }
  }

  auto _split(node::id node_id, const math::volume& bounds) -> void {
    const auto current_size = _nodes.size();

    for (auto&& [i, child] : ranges::views::enumerate(_nodes[node_id].children)) {
      child = static_cast<node::id>(current_size + i);
    }

    _nodes.insert(_nodes.end(), _nodes[node_id].children.size(), node{});

    auto new_values = std::vector<typename node::value_type>{};

    for (const auto& [value, child_bounds] : _nodes[node_id].values) {
      const auto quadrant = node::find_volume(bounds, child_bounds);

      if (quadrant) {
        const auto child_id = _nodes[node_id].child_at(*quadrant);
        _nodes[child_id].values.push_back({value, child_bounds});
      } else {
        new_values.push_back({value, child_bounds});
      }
    }

    _nodes[node_id].values = new_values;
  }

  auto _intersections(node::id node_id, const math::volume& bounds, std::vector<intersection>& intersections) -> void {
    for (auto i : std::views::iota(0u, _nodes[node_id].values.size())) {
      for (auto j : std::views::iota(0u, i)) {
        if (_nodes[node_id].values[i].bounds.intersects(_nodes[node_id].values[j].bounds)) {
          intersections.push_back({_nodes[node_id].values[i].value, _nodes[node_id].values[j].value});
        }
      }
    }

    if (!_nodes[node_id].is_leaf()) {
      for (const auto& child_id : _nodes[node_id].children) {
        for (const auto& [value, bounds] : _nodes[node_id].values) {
          _intersections_with_descendants(child_id, value, bounds, intersections);
        }
      }

      for (auto i : std::views::iota(0u, _nodes[node_id].children.size())) {
        auto child_bounds = node::child_bounds(bounds, i);

        _intersections(_nodes[node_id].child_at(i), child_bounds, intersections);
      }
    }
  }

  auto _intersections_with_descendants(node::id node_id, const value_type& value, const math::volume& value_bounds, std::vector<intersection>& intersections) -> void {
    for (const auto& [node_value, bound] :  _nodes[node_id].values) {
      if (bound.intersects(value_bounds)) {
        intersections.push_back({value, node_value});
      }
    }

    if (!_nodes[node_id].is_leaf()) {
      for (const auto& child : _nodes[node_id].children) {
        _intersections_with_descendants(child, value, value_bounds, intersections);
      }
    }
  }

  math::volume _bounds;
  node::id _root;
  std::vector<node> _nodes;

}; // class octree

} // namespace sbx::containers

#endif // LIBSBX_CONTAINERS_OCTREE_HPP_
