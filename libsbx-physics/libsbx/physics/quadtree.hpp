#ifndef LIBSBX_PHYSICS_QUADTREE_HPP_
#define LIBSBX_PHYSICS_QUADTREE_HPP_

#include <libsbx/utility/enum.hpp>

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

template<typename Type, std::size_t Threshold = 16u, std::size_t Depth = 8u>
class quadtree {

  inline static constexpr auto threshold = Threshold;
  inline static constexpr auto depth = Depth;

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;

  quadtree(const box& bounds) noexcept
  : _bounds{bounds} { }

  auto insert(const value_type& value, const box& bounds) noexcept -> void {
    
  }

private:

  class node {

  public:
    
    using id = std::uint32_t;

    inline constexpr static auto null = static_cast<id>(-1);

    /**
     * +---+---+
     * | 0 | 1 |
     * +---+---+
     * | 3 | 2 |
     * +---+---+
     */
    enum class quadrant : std::uint8_t {
      top_left = 0,
      top_right = 1,
      bottom_right = 2,
      bottom_left = 3
    }; // enum class quadrant

    node() noexcept
    : _children{null, null, null, null} { }

    auto child_at(const quadrant quadrant) -> id {
      return _children[utility::to_underlying(quadrant)];
    }

    auto is_leaf() const noexcept -> bool {
      return child_at(quadrant::top_left) == null;
    }

  private:

    memory::static_vector<value_type, threshold> _values;
    std::array<id, 4> _children{null, null, null, null};
  
  }; // struct node

  box _bounds;
  node::id _root{node::null};
  std::vector<node> _nodes;

}; // class quadtree

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_QUADTREE_HPP_
