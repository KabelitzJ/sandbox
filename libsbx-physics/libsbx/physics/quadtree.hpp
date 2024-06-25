#ifndef LIBSBX_PHYSICS_QUADTREE_HPP_
#define LIBSBX_PHYSICS_QUADTREE_HPP_

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/uuid.hpp>
#include <libsbx/memory/static_vector.hpp>

namespace sbx::physics {

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

  constexpr auto intersects(const box& other) const noexcept -> bool {
    return _min.x() <= other._max.x() && _max.x() >= other._min.x() && _min.y() <= other._max.y() && _max.y() >= other._min.y();
  }

private:

  math::vector2 _min;
  math::vector2 _max;

}; // class box

template<typename Type, std::size_t Threshold, std::size_t Depth>
class quadtree {

  inline static constexpr auto threshold = Threshold;
  inline static constexpr auto depth = Depth;

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;

  quadtree(const box_type& bounds) noexcept
  : _bounds{bounds} { }

  auto insert(const value_type& value, const box_type& bounds) noexcept -> void {
    _insert(_root, _bounds, value, bounds, 0u);
  }

private:

  /**
   * 
   * +---+---+
   * | 0 | 1 |
   * +---+---+
   * | 3 | 2 |
   * +---+---+
   */
  struct node {
    
    using id = std::uint32_t;

    inline constexpr static auto null = static_cast<id>(-1);

    memory::static_vector<value_type, threshold> values;
    std::array<id, 4> children{null, null, null, null};

    auto is_leaf() const noexcept -> bool {
      return children[0] == null;
    }

    auto child_bounds(const box& bounds, const std::size_t index) const noexcept -> box {
      const auto center = bounds.center();
      const auto size = bounds.size() / static_cast<scalar_type>(2);

      switch (index) {
        case 0: return box{math::vector2{center.x() - size.x(), center.y() + size.y()};
        case 1: return box{center, bounds.max()};
        case 2: return box{math::vector2{center.x(), bounds.min().y()}, math::vector2{bounds.max().x(), center.y()}};
        case 3: return box{math::vector2{bounds.min().x(), center.y()}, math::vector2{center.x(), bounds.max().y()}};
      }
    }

    auto quadrant(const box& bounds, const box& point) const noexcept -> std::size_t {
      const auto center = bounds.center();

      if (point.max().x() < center.x()) {
        if (point.max().y() < center.y()) return 0;
        if (point.min().y() > center.y()) return 3;
      }
      if (point.min().x() > center.x()) {
        if (point.min().y() > center.y()) return 2;
        if (point.max().y() < center.y()) return 1;
      }

      return 4;
    }
  
  }; // struct node

  auto _insert(node::id& id, const box& bounds, const value_type& value, const box& value_bounds, const std::size_t current_depth) noexcept -> void {
    if (id == node::null) {
      id = _nodes.size();
      _nodes.push_back(node{});
    }

    auto& current = _nodes[id];

    if (current.is_leaf()) {
      if (current.values.size() < threshold || current_depth == depth) {
        current.values.push_back(value);
        return;
      }

      for (const auto& current_value : current.values) {
        const auto index = current.quadrant(bounds, value_bounds);
        _insert(current.children[index], current.child_bounds(bounds, index), current_value, value_bounds, current_depth + 1);
      }

      current.values.clear();
      current.values.push_back(value);
      return;
    }

    const auto index = current.quadrant(bounds, value_bounds);
    _insert(current.children[index], current.child_bounds(bounds, index), value, value_bounds, current_depth + 1);
  }

  box _bounds;
  node::id _root{node::null};
  std::vector<node> _nodes;

}; // class quadtree

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_QUADTREE_HPP_
