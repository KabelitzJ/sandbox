#include <libsbx/physics/collider.hpp>

#include <array>

namespace sbx::physics {

static auto bounding_volume(const sphere& sphere, const math::vector3& position) -> volume {
  return volume{position - math::vector3{sphere.radius, sphere.radius, sphere.radius}, position + math::vector3{sphere.radius, sphere.radius, sphere.radius}};
}

static auto bounding_volume(const cylinder& cylinder, const math::vector3& position) -> volume {
  return volume{position - math::vector3{cylinder.radius, cylinder.base, cylinder.radius}, position + math::vector3{cylinder.radius, cylinder.cap, cylinder.radius}};
}

static auto bounding_volume(const capsule& capsule, const math::vector3& position) -> volume {
  return volume{position - math::vector3{capsule.radius, capsule.base - capsule.radius, capsule.radius}, position + math::vector3{capsule.radius, capsule.cap + capsule.radius, capsule.radius}};
}

static auto bounding_volume(const box& box, const math::vector3& position) -> volume {
  return volume{position + box.min, position + box.max};
}

auto bounding_volume(const collider& collider, const math::vector3& position) -> volume {
  return std::visit([&](const auto& shape) { return bounding_volume(shape, position); }, collider);
}

static auto find_furthest_point(const math::vector3& direction, const box& box, const math::vector3& position, const math::matrix4x4& rotation_scale) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(rotation_scale);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  auto result = math::vector3{};

  result.x() = (local_direction.x() > 0.0f) ? box.max.x() : box.min.x();
  result.y() = (local_direction.y() > 0.0f) ? box.max.y() : box.min.y();
  result.z() = (local_direction.z() > 0.0f) ? box.max.z() : box.min.z();

  return math::vector3{rotation_scale * math::vector4{result, 1.0f}} + position;
}

static auto find_furthest_point(const math::vector3& direction, const sphere& sphere, const math::vector3& position, const math::matrix4x4& rotation_scale) -> math::vector3 {
  return math::vector3::normalized(direction) * sphere.radius + position;
}

static auto find_furthest_point(const math::vector3& direction, const cylinder& cylinder, const math::vector3& position, const math::matrix4x4& rotation_scale) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(rotation_scale);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  const auto local_direction_xz = math::vector3{local_direction.x(), 0.0f, local_direction.z()};

  auto result = math::vector3::normalized(local_direction_xz) * cylinder.radius;
  result.y() = (local_direction.y() > 0.0f) ? cylinder.cap : cylinder.base;

  return math::vector3{rotation_scale * math::vector4{result, 1.0f}} + position;
}

static auto find_furthest_point(const math::vector3& direction, const capsule& capsule, const math::vector3& position, const math::matrix4x4& rotation_scale) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(rotation_scale);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  auto result = math::vector3::normalized(local_direction) * capsule.radius;
  result.y() = (local_direction.y() > 0.0f) ? capsule.cap : capsule.base;

  return math::vector3{rotation_scale * math::vector4{result, 1.0f}} + position;
}

auto find_furthest_point(const collider_data& data, const math::vector3& direction) -> math::vector3 {
  return std::visit([&](const auto& shape) { return find_furthest_point(direction, shape, data.position, data.rotation_scale); }, data.collider);
}

auto support(const collider_data& first, const collider_data& second, const math::vector3& direction) -> math::vector3 {
  return find_furthest_point(first, direction) - find_furthest_point(second, -direction);
}

class simplex {

public:

  using value_type = math::vector3;
  using reference = value_type;
  using const_iterator = const value_type*;

  simplex()
  : _size{0} { }

	auto operator=(std::initializer_list<value_type> list) -> simplex& {
		_size = 0u;

		for (const auto& point : list) {
			_points[_size++] = point;
    }

		return *this;
	}

	auto push_front(const value_type& point) -> void {
		_points = { point, _points[0], _points[1], _points[2] };
		_size = std::min(_size + 1u, std::size_t{4});
	}

	auto operator[](const std::size_t index) -> reference { 
    return _points[index]; 
  }

	auto size() const noexcept -> std::size_t { 
    return _size; 
  }

	auto begin() const -> const_iterator { 
    return _points.begin(); 
  }

	auto end() const -> const_iterator {
    return _points.end() - (4u - _size); 
  }

private:

  std::array<math::vector3, 4u> _points;
  std::size_t _size;

}; // class simplex

static auto same_direction(const math::vector3& lhs, const math::vector3& rhs) -> bool {
  return math::vector3::dot(lhs, rhs) > 0.0f;
}

static auto line(simplex& simplex, math::vector3& direction) -> bool {
  auto a = simplex[0];
	auto b = simplex[1];

	auto ab = b - a;
	auto ao = -a;
 
	if (same_direction(ab, ao)) {
		direction = math::vector3::cross(math::vector3::cross(ab, ao), ab);
	} else {
		simplex = { a };
		direction = ao;
	}

	return false;
}

static auto triangle(simplex& simplex, math::vector3& direction) -> bool {
  auto a = simplex[0];
	auto b = simplex[1];
	auto c = simplex[2];

	auto ab = b - a;
	auto ac = c - a;
	auto ao =   - a;
 
	auto abc = math::vector3::cross(ab, ac);
 
	if (same_direction(math::vector3::cross(abc, ac), ao)) {
		if (same_direction(ac, ao)) {
			simplex = { a, c };
			direction = math::vector3::cross(math::vector3::cross(ac, ao), ac);
		} else {
			return line(simplex = { a, b }, direction);
		}
	} else {
		if (same_direction(math::vector3::cross(ab, abc), ao)) {
			return line(simplex = { a, b }, direction);
		} else {
			if (same_direction(abc, ao)) {
				direction = abc;
			} else {
				simplex = { a, c, b };
				direction = -abc;
			}
		}
	}

	return false;
}

static auto tetrahedron(simplex& simplex, math::vector3& direction) -> bool {
  auto a = simplex[0];
	auto b = simplex[1];
	auto c = simplex[2];
	auto d = simplex[3];

	auto ab = b - a;
	auto ac = c - a;
	auto ad = d - a;
	auto ao =   - a;
 
	auto abc = math::vector3::cross(ab, ac);
	auto acd = math::vector3::cross(ac, ad);
	auto adb = math::vector3::cross(ad, ab);
 
	if (same_direction(abc, ao)) {
		return triangle(simplex = { a, b, c }, direction);
	}
		
	if (same_direction(acd, ao)) {
		return triangle(simplex = { a, c, d }, direction);
	}
 
	if (same_direction(adb, ao)) {
		return triangle(simplex = { a, d, b }, direction);
	}
 
	return true;
}

static auto next_simplex(simplex& simplex, math::vector3& direction) -> bool {
  switch (simplex.size()) {
    case 2u: return line(simplex, direction);
    case 3u: return triangle(simplex, direction);
    case 4u: return tetrahedron(simplex, direction);
  }

  return false;
}

template<math::unsigned_integral Start, math::unsigned_integral End, math::unsigned_integral Step>
static auto stepped_iota(const Start start, const End end, const Step step = Step{1u}) {
  return std::ranges::views::iota(0u, (end - start + step - 1) / step) | std::ranges::views::transform([=](auto x) { return x * step + start; });
}

auto get_face_normals(const std::vector<math::vector3>& polytope, const std::vector<std::size_t>& faces) -> std::pair<std::vector<math::vector4>, std::size_t> {
	auto normals = std::vector<math::vector4>{};

	auto min_triangle = std::size_t{0};
	auto min_distance = std::numeric_limits<std::float_t>::max();

	for (auto i : stepped_iota(0u, faces.size(), 3u)) {
		auto a = polytope[faces[i + 0u]];
		auto b = polytope[faces[i + 1u]];
		auto c = polytope[faces[i + 2u]];

		auto normal = math::vector3::normalized(math::vector3::cross(b - a, c - a));
		auto distance = math::vector3::dot(normal, a);

		if (distance < 0) {
			normal *= -1.0f;
			distance *= -1.0f;
		}

		normals.emplace_back(normal, distance);

		if (distance < min_distance) {
			min_triangle = i / 3u;
			min_distance = distance;
		}
	}

	return {normals, min_triangle};
}

auto add_if_unique_edge(std::vector<std::pair<std::size_t, std::size_t>>& edges, const std::vector<std::size_t>& faces, std::size_t a, std::size_t b) -> void {
	auto reverse = std::find(                     //      0--<--3
		edges.begin(),                              //     / \ B /   A: 2-0
		edges.end(),                                //    / A \ /    B: 0-2
		std::make_pair(faces[b], faces[a])          //   1-->--2
	);
 
	if (reverse != edges.end()) {
		edges.erase(reverse);
	} else {
		edges.emplace_back(faces[a], faces[b]);
	}
}

static auto epa(const simplex& simplex, const collider_data& first, const collider_data& second) -> std::optional<math::vector3> {
  auto polytope = std::vector<math::vector3>{simplex.begin(), simplex.end()};

  auto faces = std::vector<std::size_t>{
    0u, 1u, 2u,
		0u, 3u, 1u,
		0u, 2u, 3u,
		1u, 3u, 2u
  };

  auto [normals, min_face] = get_face_normals(polytope, faces);

  auto min_normal = math::vector3{};
	auto min_distance = std::numeric_limits<std::float_t>::max();
	
	auto iterations = 0u;

  while (min_distance == std::numeric_limits<std::float_t>::max()) {
    min_normal   = math::vector3{normals[min_face]};
    min_distance = normals[min_face].w();

    if (iterations++ > 32u) {
      break;
    }

    auto support = physics::support(first, second, min_normal);
    auto distance = math::vector3::dot(min_normal, support);

    if (std::abs(distance - min_distance) > 0.001f) {
      min_distance = std::numeric_limits<std::float_t>::max();

      auto unique_edges = std::vector<std::pair<std::size_t, std::size_t>>{};

      for (auto i = 0u; i < normals.size(); ++i) {
        if (same_direction(math::vector3{normals[i]}, support)) {
          auto f = i * 3u;

          add_if_unique_edge(unique_edges, faces, f + 0u, f + 1u);
          add_if_unique_edge(unique_edges, faces, f + 1u, f + 2u);
          add_if_unique_edge(unique_edges, faces, f + 2u, f + 0u);

          faces[f + 2u] = faces.back(); faces.pop_back();
          faces[f + 1u] = faces.back(); faces.pop_back();
          faces[f + 0u] = faces.back(); faces.pop_back();

          normals[i] = normals.back(); normals.pop_back();

          i--;
        }
      }

      if (unique_edges.size() == 0u) {
        break;
      }

      auto new_faces = std::vector<std::size_t>{};

      for (auto [edge1, edge2] : unique_edges) {
        new_faces.push_back(edge1);
        new_faces.push_back(edge2);
        new_faces.push_back(polytope.size());
      }

      polytope.push_back(support);

      auto [new_normals, new_min_face] = get_face_normals(polytope, new_faces);

      float new_min_distance = std::numeric_limits<std::float_t>::max();

      for (auto i : std::views::iota(0u, normals.size())) {
        if (normals[i].w() < new_min_distance) {
          new_min_distance = normals[i].w();
          min_face = i;
        }
      }

      if (new_normals[new_min_face].w() < new_min_distance) {
        min_face = new_min_face + normals.size();
      }

      faces.insert(faces.end(), new_faces.begin(), new_faces.end());
      normals.insert(normals.end(), new_normals.begin(), new_normals.end());
    }
  }

  if (min_distance == std::numeric_limits<std::float_t>::max()) {
    return std::nullopt;
  }

  return min_normal * (min_distance + 0.001f);
}

auto gjk(const collider_data& first, const collider_data& second) -> std::optional<math::vector3> {
  auto simplex = physics::simplex{};

  auto support = physics::support(first, second, math::vector3{1.0f, 0.0f, 0.0f});

  simplex.push_front(support);

  auto direction = -support;

  for ([[maybe_unused]] auto iteration : std::views::iota(0u, 32u)) {
    support = physics::support(first, second, direction);

    if (math::vector3::dot(support, direction) <= 0.0f) {
      return std::nullopt;
    }

    simplex.push_front(support);

    if (next_simplex(simplex, direction)) {
      return epa(simplex, first, second);
    }
  }

  return std::nullopt;
}

} // namespace sbx::physics
