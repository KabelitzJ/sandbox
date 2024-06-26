#ifndef LIBSBX_PHYSICS_QUADTREE_HPP_
#define LIBSBX_PHYSICS_QUADTREE_HPP_

#include <libsbx/utility/enum.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/uuid.hpp>

#include <libsbx/memory/static_vector.hpp>

namespace sbx::physics {

/**
 *        max
 * +--------+
 * |        |
 * |        |
 * |        |
 * +--------+
 * min
 */
class box {

public:

  constexpr box(const math::vector2& min, const math::vector2& max) noexcept
  : _min{min}, _max{max} { }

  constexpr auto min() const noexcept -> const math::vector2& {
    return _min;
  }

  constexpr auto max() const noexcept -> const math::vector2& {
    return _max;
  }

  constexpr auto center() const noexcept -> math::vector2 {
    return (_min + _max) / static_cast<std::float_t>(2);
  }

  constexpr auto size() const noexcept -> math::vector2 {
    return _max - _min;
  }

  constexpr auto contains(const math::vector2& point) const noexcept -> bool {
    return point.x() >= _min.x() && point.x() <= _max.x() && point.y() >= _min.y() && point.y() <= _max.y();
  }

  constexpr auto contains(const box& other) const noexcept -> bool {
    return _min.x() <= other._min.x() && _min.y() <= other._min.y() && _max.x() >= other._max.x() && _max.y() >= other._max.y();
  }

  constexpr auto intersects(const box& other) const noexcept -> bool {
    return _min.x() <= other._max.x() && _max.x() >= other._min.x() && _min.y() <= other._max.y() && _max.y() >= other._min.y();
  }

private:

  math::vector2 _min;
  math::vector2 _max;

}; // class box

template<typename Type, std::size_t Threshold = 16u, std::size_t Depth = 8u>
class quadtree {

  inline static constexpr auto threshold = Threshold;
  inline static constexpr auto depth = Depth;

  struct node {
    
    using id = std::uint32_t;

    inline constexpr static auto null = static_cast<id>(-1);

    /**
     * +---+---+
     * | 0 | 1 |
     * +---+---+
     * | 2 | 3 |
     * +---+---+
     */
    enum class quadrant : std::uint8_t {
      top_left = 0,
      top_right = 1,
      bottom_left = 2,
      bottom_right = 3
    }; // enum class quadrant

    node() noexcept
    : children{null, null, null, null} { }

    auto child_at(const quadrant quadrant) const noexcept -> id {
      return children[utility::to_underlying(quadrant)];
    }

    auto is_leaf() const noexcept -> bool {
      return child_at(quadrant::top_left) == null;
    }

    auto push_back(std::pair<Type, box>&& value) -> void {
      values.push_back(std::move(value));
    }

    static auto child_box(const box& box, const quadrant quadrant) -> physics::box {
      const auto& min = box.min();
      const auto& max = box.max();
      const auto& center = box.center();

      switch (quadrant) {
        case quadrant::top_left: {
          return physics::box{math::vector2{min.x(), center.y()}, math::vector2{center.x(), max.y()}};
        }
        case quadrant::top_right: {
          return physics::box{math::vector2{center.x(), center.y()}, math::vector2{max.x(), max.y()}};
        }
        case quadrant::bottom_left: {
          return physics::box{math::vector2{min.x(), min.y()}, math::vector2{center.x(), center.y()}};
        }
        case quadrant::bottom_right: {
          return physics::box{math::vector2{center.x(), min.y()}, math::vector2{max.y(), center.y()}};
        }
      }

      throw std::runtime_error{"invalid value for enum quadrant"};
    }

    static auto find_quadrant(const box& outer, const box& inner) noexcept -> std::optional<quadrant> {
      auto center = outer.center();

      if (inner.min().x() < center.x()) {
        if (inner.min().y() < center.y()) {
          return quadrant::top_left;
        } 
        
        if (inner.max().y() >= center.y()) {
          return quadrant::bottom_left;
        } 
        
        return std::nullopt;
      } 
      
      if (inner.min().x() >= center.x()) {
        if (inner.min().y() < center.y()) {
          return quadrant::top_right;
        } 
        
        if (inner.max().y() >= center.y()) {
          return quadrant::bottom_left;
        }
        
        return std::nullopt;
      }
      
      return std::nullopt;
    }

    memory::static_vector<std::pair<Type, box>, threshold> values;
    std::array<id, 4> children{null, null, null, null};
  
  }; // struct node

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;

  quadtree(const box& bounds) noexcept
  : _bounds{bounds} { 
    _root = 0u;
    _nodes.push_back(node{});
  }

  auto insert(const value_type& value, const box& bounds) noexcept -> void {
    _insert(_root, _bounds, value, bounds, 0u);
  }

private:

  void _insert(node::id node_id, const box& bounds, const value_type& value, const box& value_bounds, std::size_t current_depth) {
    if (!bounds.contains(value_bounds)) {
      return;
    }

    auto& node = _nodes[node_id];

    if (node.is_leaf()) {
      if (node.values.size() < Threshold || current_depth >= depth) {
        node.push_back({value, value_bounds});
      } else {
        _split(node_id, bounds);
        _insert(node_id, bounds, value, value_bounds, current_depth);
      }
    } else {
      const auto quadrant = node::find_quadrant(bounds, value_bounds);

      if (quadrant) {
        _insert(node.child_at(*quadrant), node::child_box(bounds, *quadrant), value, value_bounds, current_depth + 1u);
      } else {
        node.values.push_back({value, value_bounds});
      }
    }
  }

  auto _split(node::id node_id, const box& bounds) -> void {
    auto& parent_node = _nodes[node_id];

    for (auto& child_id : parent_node.children) {
      child_id = static_cast<node::id>(_nodes.size());
      _nodes.push_back(node{});
    }

    auto new_values = memory::static_vector<std::pair<Type, box>, threshold>{};

    for (const auto& [value, child_bounds] : parent_node.values) {
      const auto quadrant = node::find_quadrant(bounds, child_bounds);

      if (quadrant) {
        _nodes[parent_node.child_at(*quadrant)].values.push_back({value, child_bounds});
      } else {
        new_values.push_back({value, child_bounds});
      }
    }

    parent_node.values = new_values;
  }

  box _bounds;
  node::id _root;
  std::vector<node> _nodes;

}; // class quadtree

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_QUADTREE_HPP_
