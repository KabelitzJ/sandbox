#ifndef DEMO_TERRAIN_VORONOI_HPP_
#define DEMO_TERRAIN_VORONOI_HPP_

#include <vector>
#include <memory>
#include <list>
#include <array>

#include <libsbx/math/vector2.hpp>

namespace demo {

template<typename Type>
concept priority_queue_element = requires(Type t) {
  { t.index } -> std::same_as<std::size_t>;
  { t < t } -> std::same_as<bool>;
}; // concept priority_queue_element

template<priority_queue_element Type>
class priority_queue {

public:

  using value_type = Type;

  priority_queue() {

  }

  auto is_empty() const -> bool {
    return _elements.empty();
  }

  auto pop() -> std::unique_ptr<value_type> {
    if (_elements.empty()) {
      return nullptr;
    }

    _swap(0, _elements.size() - 1u);
    auto top = std::move(_elements.back());
    _elements.pop_back();
    _shift_down(0u);

    return top;
  }

  auto push(std::unique_ptr<value_type> element) -> void {
    element->index = _elements.size();
    _elements.push_back(std::move(element));
    _shift_up(_elements.size() - 1u);
  }

  auto update(const std::size_t index) -> void {
    const auto parent = _parent(index);

    if (parent >= 0 && *_elements[parent] < *_elements[index]) {
      _shift_up(index);
    } else {
      _shift_down(index);
    }
  }

  auto remove(const std::size_t index) -> void {
    _swap(index, _elements.size() - 1u);
    _elements.pop_back();
    
    if (index < _elements.size()) {
      update(index);
    }
  }

private:

  auto _parent(const std::size_t index) const -> std::size_t {
    return (index - 1u) / 2u;
  }

  auto _left_child(const std::size_t index) const -> std::size_t {
    return 2u * (index + 1u) - 1u;
  }

  auto _right_child(const std::size_t index) const -> std::size_t {
    return 2u * (index + 1u);
  }

  auto _shift_down(const std::size_t index) -> void {
    const auto left = _left_child(index);
    const auto right = _right_child(index);
    auto largest = index;

    if (left < _elements.size() && _elements[left]->priority() > _elements[largest]->priority()) {
      largest = left;
    }

    if (right < _elements.size() && _elements[right]->priority() > _elements[largest]->priority()) {
      largest = right;
    }

    if (largest != index) {
      _swap(_elements[index], _elements[largest]);
      _shift_down(largest);
    }
  }

  auto _shift_up(const std::size_t index) -> void {
    const auto parent = _parent(index);

    if (parent >= 0 && *_elements[parent] < *_elements[index]) {
      _swap(index, parent);
      _shift_up(parent);
    }
  }

  auto _swap(const std::size_t left, const std::size_t right) -> void {
    auto temp = std::move(_elements[left]);
    _elements[left] = std::move(_elements[right]);
    _elements[right] = std::move(temp);

    _elements[left]->index = left;
    _elements[right]->index = right;
  }

  std::vector<std::unique_ptr<value_type>> _elements;

}; // class priority_queue

struct face;
struct site;
struct half_edge;
struct arc;

struct site {
  sbx::math::vector2 position;
  std::size_t index;
  std::shared_ptr<face> face;
}; // struct site

struct point {
  sbx::math::vector2 position;
  std::list<vertex>::iterator iterator;
}; // struct point

struct half_edge {
  std::shared_ptr<point> start;
  std::shared_ptr<point> end;
  std::shared_ptr<half_edge> twin;
  std::shared_ptr<face> face;
  std::shared_ptr<half_edge> previous;
  std::shared_ptr<half_edge> next;
  std::list<half_edge>::iterator iterator;
}; // struct half_edge

struct face {
  std::shared_ptr<site> site;
  std::shared_ptr<half_edge> edge;
}; // struct face

struct site_event {

  std::float_t y;
  std::int32_t index;
  std::shared_ptr<site> site;

  site_event(const std::shared_ptr<demo::site>& s)
  : y{s->position.y()},
    site{s},
    index{-1} { }
  
}; // struct site_event

struct circle_event {

  std::float_t y;
  std::int32_t index;
  sbx::math::vector2 center;
  std::shared_ptr<arc> arc;

  circle_event(const std::float_t y_position, const sbx::math::vector2& c, const std::shared_ptr<demo::arc>& a)
  : y{y_position},
    center{c},
    arc{a},
    index{-1} { }

}; // struct circle_event

using event = std::variant<site_event, circle_event>;

auto operator<(const event& left, const event& right) -> bool {
  return std::visit([](const auto& l, const auto& r) {
    return l.y < r.y;
  }, left, right);
}

struct arc {
 
  enum class color : std::uint8_t {
    red,
    black
  }; // enum class color

  std::shared_ptr<arc> parent;
  std::shared_ptr<arc> left;
  std::shared_ptr<arc> right;

  std::shared_ptr<site> site;
  std::shared_ptr<half_edge> left_half_edge;
  std::shared_ptr<half_edge> right_half_edge;
  std::shared_ptr<event> event;

  std::shared_ptr<arc> previous;
  std::shared_ptr<arc> next;

  color color;

}; // struct arc

struct box {

  inline static constexpr auto epsilon = std::numeric_limits<std::float_t>::epsilon();

  enum class side : std::uint8_t {
    top,
    right,
    bottom,
    left
  }; // enum class side

  struct intersection {
    side side;
    sbx::math::vector2 position;
  }; // struct intersection

  std::float_t left;
  std::float_t right;
  std::float_t top;
  std::float_t bottom;

  auto contains(const sbx::math::vector2& point) const -> bool {
    return point.x() >= left - epsilon && point.x() <= right + epsilon && point.y() >= top - epsilon && point.y() <= bottom + epsilon;
  }

  auto first_intersection(const sbx::math::vector2& origin, const sbx::math::vector2& direction) -> intersection {
    auto result = intersection{};

    auto t = std::numeric_limits<std::float_t>::infinity();

    if (direction.x() > 0.0f) {
      t = (right - origin.x()) / direction.x();
      result.side = side::right;
      result.position = origin + direction * t;
    } else if (direction.x() < 0.0f) {
      t = (left - origin.x()) / direction.x();
      result.side = side::left;
      result.position = origin + direction * t;
    }

    if (direction.y() > 0.0f) {
      auto new_t = (top - origin.y()) / direction.y();

      if (new_t < t) {
        result.side = side::top;
        result.position = origin + direction * new_t;
      }
    } else if (direction.y() < 0.0f) {
      auto new_t = (bottom - origin.y()) / direction.y();

      if (new_t < t) {
        result.side = side::bottom;
        result.position = origin + direction * new_t;
      }
    }

    return result;
  }

  auto intersections(const sbx::math::vector2& origin, const sbx::math::vector2& destination) -> std::vector<intersection> {
    auto result = std::vector<intersection>{};

    const auto direction = destination - origin;
    auto t = std::array<std::float_t, 2u>{};

    if (origin.x() < left || destination.x() < left - epsilon) {
      const auto i = result.size();

      t[i] = (left - origin.x()) / direction.x();
    }
  }

}; // class box

// https://pvigier.github.io/2018/11/18/fortune-algorithm-details.html

} // namespace demo

#endif // DEMO_TERRAIN_VORONOI_HPP_
