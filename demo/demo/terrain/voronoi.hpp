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

    // Left
    if (origin.x() < left || destination.x() < left - epsilon) {
      const auto i = result.size();

      t[i] = (left - origin.x()) / direction.x();

      if (t[i] > epsilon && t[i] < 1.0 - epsilon) {
        const auto position = origin + direction * t[i];

        if (position.y() >= bottom - epsilon && position.y() <= top + epsilon) {
          result.push_back(intersection{side::left, position});
        }
      }
    }

    // Right
    if (origin.x() > right || destination.x() > right + epsilon) {
      const auto i = result.size();

      t[i] = (right - origin.x()) / direction.x();

      if (t[i] > epsilon && t[i] < 1.0 - epsilon) {
        const auto position = origin + direction * t[i];

        if (position.y() >= bottom - epsilon && position.y() <= top + epsilon) {
          result.push_back(intersection{side::right, position});
        }
      }
    }

    // Bottom
    if (origin.y() < bottom || destination.y() < bottom - epsilon) {
      const auto i = result.size();

      t[i] = (bottom - origin.y()) / direction.y();

      if (t[i] > epsilon && t[i] < 1.0 - epsilon) {
        const auto position = origin + direction * t[i];

        if (position.x() >= left - epsilon && position.x() <= right + epsilon) {
          result.push_back(intersection{side::bottom, position});
        }
      }
    }

    // Top
    if (origin.y() > top || destination.y() > top + epsilon) {
      const auto i = result.size();

      t[i] = (top - origin.y()) / direction.y();

      if (t[i] > epsilon && t[i] < 1.0 - epsilon) {
        const auto position = origin + direction * t[i];

        if (position.x() >= left - epsilon && position.x() <= right + epsilon) {
          result.push_back(intersection{side::top, position});
        }
      }
    }

    // Sort the intersections by distance
    if (result.size() == 2u && t[0] > t[1]) {
      std::swap(result[0], result[1]);
    }

    return result;
  }

}; // class box

class beach_line {

public:

  beach_line()
  : _nil{std::make_shared<arc>()},
    _root{_nil} {
    _nil->color = arc::color::black;
  }

  beach_line(const beach_line&) = delete;
  beach_line(beach_line&&) = delete;

  auto operator=(const beach_line&) -> beach_line& = delete;
  auto operator=(beach_line&&) -> beach_line& = delete;

  auto create_arc(const std::shared_ptr<site>& site) -> std::shared_ptr<arc> {
    return std::make_shared<arc>(_nil, _nil, _nil, site, nullptr, nullptr, nullptr, _nil, _nil, arc::color::red);
  }

  auto is_empty() const -> bool {
    return is_nil(_root);
  }

  auto is_nil(const std::shared_ptr<site>& node) const -> bool {
    return node == _nil;
  }

  auto set_root(const std::shared_ptr<arc>& node) -> void {
    _root = node;
    _root->color = arc::color::black;
  }

  auto left_most_arc() -> std::shared_ptr<arc> {
    auto node = _root;

    while (!is_nil(node->previous)) {
      node = node->previous;
    }

    return node;
  }

  auto arc_above(const sbx::math::vector2& point, const std::float_t y) -> std::shared_ptr<arc> {
    auto node = _root;
    auto found = false;

    while (!found) {
      auto left_breakpoint = -std::numeric_limits<std::float_t>::infinity();
      auto right_breakpoint = std::numeric_limits<std::float_t>::infinity();

      if (!is_nil(node->previous)) {
        left_breakpoint = _break_point(node->previous->site->position, node->site->position, y);
      }

      if (!is_nil(node->next)) {
        right_breakpoint = _break_point(node->site->position, node->next->site->position, y);
      }

      if (point.x() < left_breakpoint) {
        node = node->left;
      } else if (point.x() > right_breakpoint) {
        node = node->right;
      } else {
        found = true;
      }
    }

    return node;
  }

  auto insert_before(const std::shared_ptr<arc>& x, const std::shared_ptr<arc>& y) -> void {
    if (is_nil(x->left)) {
      x->left = y;
      y->parent = x;
    } else {
      x->prev->right = y;
      y->parent = x->prev;
    }

    y->previous = x->previous;

    if (!is_nil(y->previous)) {
      y->previous->next = y;
    }

    y->next = x;
    x->previous = y;

    _insert_fixup(y);
  }

  auto insert_after(const std::shared_ptr<arc>& x, const std::shared_ptr<arc>& y) -> void {
    if (is_nil(x->right)) {
      x->right = y;
      y->parent = x;
    } else {
      x->next->left = y;
      y->parent = x->next;
    }

    y->next = x->next;

    if (!is_nil(y->next)) {
      y->next->previous = y;
    }

    y->previous = x;
    x->next = y;

    _insert_fixup(y);   
  }

  auto replace(const std::shared_ptr<arc>& x, const std::shared_ptr<arc>& y) -> void {
    _transplant(x, y);

    y->left = x->left;
    y->right = x->right;
  
    if (!is_nil(y->left)) {
      y->left->parent = y;
    }

    if (!is_nil(y->right)) {
      y->right->parent = y;
    }

    y->previous = x->previous;
    y->next = x->next;

    if (!is_nil(y->previous)) {
      y->previous->next = y;
    }

    if (!is_nil(y->next)) {
      y->next->previous = y;
    }

    y->color = x->color;
  }

  auto remove(const std::shared_ptr<arc>& z) -> void {
    auto y = z;
    auto original_color = y->color;
    auto x = std::shared_ptr<arc>{};

    if (is_nil(z->left)) {
      x = z->right;
      _transplant(z, z->right);
    } else if (is_nil(z->right)) {
      x = z->left;
      _transplant(z, z->left);
    } else {
      y = _minimum(z->right);
      original_color = y->color;
      x = y->right;

      if (y->parent == z) {
        x->parent = y;
      } else {
        _transplant(y, y->right);
        y->right = z->right;
        y->right->parent = y;
      }

      _transplant(z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
    }

    if (original_color == arc::color::black) {
      _remove_fixup(x);
    }

    if (!is_nil(z->previous)) {
      z->previous->next = z->next;
    }

    if (!is_nil(z->next)) {
      z->next->previous = z->previous;
    }
  }

private:

  auto _minimum(const std::shared_ptr<arc>& node) -> std::shared_ptr<arc> {
    auto current = node;

    while (!is_nil(current->left)) {
      current = current->left;
    }

    return current;
  }

  auto _transplant(const std::shared_ptr<arc>& u, const std::shared_ptr<arc>& v) -> void {
    if (is_nil(u->parent)) {
      _root = v;
    } else (u == u->parent->left) {
      u->parent->left = v;
    } else {
      u->parent->right = v;
    }

    v->parent = u->parent;
  }

  auto _insert_fixup(const std::shared_ptr<arc>& node) -> void{

  }

  auto _remove_fixup(const std::shared_ptr<arc>& node) -> void {

  }

  auto _rotate_left(const std::shared_ptr<arc>& node) -> void {

  }

  auto _rotate_right(const std::shared_ptr<arc>& y) -> void {
    auto x = y->left;
    y->left = x->right;
    if (!isNil(x->right))
        x->right->parent = y;
    x->parent = y->parent;
    if (isNil(y->parent))
        mRoot = x;
    else if (y->parent->left == y)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
  }

  auto _break_point(const sbx::math::vector2& left, const sbx::math::vector2& right, const std::float_t y) -> std::float_t {
    auto x1 = left.x();
    auto y1 = left.y();
    auto x2 = right.x();
    auto y2 = right.y();

    auto d1 = 1.0f / (2.0f * (y1 - y));
    auto d2 = 1.0f / (2.0f * (y2 - y));

    auto a = d1 - d2;
    auto b = 2.0f * (x2 * d2 - x1 * d1);
    auto c = (y1 * y1 + x1 * x1 - l * l) * d1 - (y2 * y2 + x2 * x2 - l * l) * d2;

    auto delta = b * b - 4.0f * a * c;

    return (-b + std::sqrt(delta)) / (2.0f * a);
  }

  std::shared_ptr<arc> _nil;
  std::shared_ptr<arc> _root;

}; // class beach_line

// https://pvigier.github.io/2018/11/18/fortune-algorithm-details.html

} // namespace demo

#endif // DEMO_TERRAIN_VORONOI_HPP_
