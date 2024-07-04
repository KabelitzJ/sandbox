#include <libsbx/physics/collider.hpp>

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

static auto support(const math::vector3& direction, const box& box, const math::vector3& position, const math::matrix4x4& rotation_scale) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(rotation_scale);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  auto result = math::vector3{};

  result.x() = (local_direction.x() > 0.0f) ? box.max.x() : box.min.x();
  result.y() = (local_direction.y() > 0.0f) ? box.max.y() : box.min.y();
  result.z() = (local_direction.z() > 0.0f) ? box.max.z() : box.min.z();

  return math::vector3{rotation_scale * math::vector4{result, 1.0f}} + position;
}

static auto support(const math::vector3& direction, const sphere& sphere, const math::vector3& position, const math::matrix4x4& rotation_scale) -> math::vector3 {
  return math::vector3::normalized(direction) * sphere.radius;
}

static auto support(const math::vector3& direction, const cylinder& cylinder, const math::vector3& position, const math::matrix4x4& rotation_scale) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(rotation_scale);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  const auto local_direction_xz = math::vector3{local_direction.x(), 0.0f, local_direction.z()};

  auto result = math::vector3::normalized(local_direction_xz) * cylinder.radius;
  result.y() = (local_direction.y() > 0.0f) ? cylinder.cap : cylinder.base;

  return math::vector3{rotation_scale * math::vector4{result, 1.0f}} + position;
}

static auto support(const math::vector3& direction, const capsule& capsule, const math::vector3& position, const math::matrix4x4& rotation_scale) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(rotation_scale);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  auto result = math::vector3::normalized(local_direction) * capsule.radius;
  result.y() = (local_direction.y() > 0.0f) ? capsule.cap : capsule.base;

  return math::vector3{rotation_scale * math::vector4{result, 1.0f}} + position;
}

auto support(const math::vector3& direction, const collider_data& data) -> math::vector3 {
  return std::visit([&](const auto& shape) { return support(direction, shape, data.position, data.rotation_scale); }, data.collider);
}

static auto update_simplex3(math::vector3& a, math::vector3& b, math::vector3& c, math::vector3& d, std::uint32_t& dimension, math::vector3& direction) -> void {
  const auto n = math::vector3::cross(b - a, c - a);
  const auto ao = -a;

  dimension = 2u;

  if (math::vector3::dot(math::vector3::cross(b - a, n), ao) > 0.0f) {
    c = a;
    direction = math::vector3::cross(math::vector3::cross(b - a, ao), b - a);
    return;
  } 
  
  if (math::vector3::dot(math::vector3::cross(n, c - a), ao) > 0.0f) {
    b = a;
    direction = math::vector3::cross(math::vector3::cross(c - a, ao), c - a);
    return;
  }

  dimension = 3u;

  if (math::vector3::dot(n, ao) > 0.0f) {
    d = c;
    c = b;
    b = a;
    direction = n;
    return;
  }

  d = b;
  b = a;
  direction = -n; 
}

static auto update_simplex4(math::vector3& a, math::vector3& b, math::vector3& c, math::vector3& d, std::uint32_t& dimension, math::vector3& direction) -> bool {
  const auto abc = math::vector3::cross(b - a, c - a);
  const auto acd = math::vector3::cross(c - a, d - a);
  const auto adb = math::vector3::cross(d - a, b - a);

  const auto ao = -a;
  dimension = 3u;

  if (math::vector3::dot(abc, ao) > 0.0f) {
    d = c;
    c = b;
    b = a;
    direction = abc;
    return false;
  }

  if (math::vector3::dot(acd, ao) > 0.0f) {
    b = a;
    direction = acd;
    return false;
  }

  if (math::vector3::dot(adb, ao) > 0.0f) {
    c = d;
    d = b;
    b = a;
    direction = adb;
    return false;
  }

  return true;
}

static auto expanding_polytope(const math::vector3& a, const math::vector3& b, const math::vector3& c, const math::vector3& d, const collider_data& first, const collider_data& second) -> math::vector3 {
  static constexpr auto tollerance = 0.0001f;
  static constexpr auto bias = 0.000001f;
  static constexpr auto max_faces = 64u;
  static constexpr auto max_loose_edges = 32u;
  static constexpr auto max_iterations = 64u;

  auto faces = std::array<std::array<math::vector3, 4u>, max_faces>{};

  //Init with final simplex from GJK
  faces[0][0] = a;
  faces[0][1] = b;
  faces[0][2] = c;
  faces[0][3] = math::vector3::normalized(math::vector3::cross(b-a, c-a)); //ABC
  faces[1][0] = a;
  faces[1][1] = c;
  faces[1][2] = d;
  faces[1][3] = math::vector3::normalized(math::vector3::cross(c-a, d-a)); //ACD
  faces[2][0] = a;
  faces[2][1] = d;
  faces[2][2] = b;
  faces[2][3] = math::vector3::normalized(math::vector3::cross(d-a, b-a)); //ADB
  faces[3][0] = b;
  faces[3][1] = d;
  faces[3][2] = c;
  faces[3][3] = math::vector3::normalized(math::vector3::cross(d-b, c-b)); //BDC

  auto num_faces = 4u;
  auto closest_face = 0u;

  for (auto iteration : std::views::iota(0u, max_iterations)) {
    auto min_distance = math::vector3::dot(faces[0][0], faces[0][3]);
    closest_face = 0u;

    for (auto i : std::views::iota(1u, num_faces)) {
      const auto distance = math::vector3::dot(faces[i][0], faces[i][3]);

      if (distance < min_distance) {
        min_distance = distance;
        closest_face = i;
      }
    }

    auto search_dir = faces[closest_face][3]; 
    auto p = support(search_dir, second) - support(-search_dir, first);

    if (math::vector3::dot(p, search_dir)- min_distance < tollerance) {
      return faces[closest_face][3] * math::vector3::dot(p, search_dir);
    }

    auto loose_edges = std::array<std::array<math::vector3, 2u>, max_loose_edges>{};
    auto num_loose_edges = 0u;

    //Find all triangles that are facing p
    for (auto i : std::views::iota(0u, num_faces)) {
      if (math::vector3::dot(faces[i][3], p - faces[i][0]) > 0) {
        for (auto j : std::views::iota(0u, 3u)) {
          auto current_edge = std::array<math::vector3, 2u>{faces[i][j], faces[i][(j + 1) % 3]};
          auto found_edge = false;

          for (auto k : std::views::iota(0u, num_loose_edges)) {
            if (loose_edges[k][1]==current_edge[0] && loose_edges[k][0]==current_edge[1]){
              loose_edges[k][0] = loose_edges[num_loose_edges-1][0];
              loose_edges[k][1] = loose_edges[num_loose_edges-1][1];

              num_loose_edges--;

              found_edge = true;

              k=num_loose_edges;
            }
          }

          if (!found_edge) {
            if (num_loose_edges >= max_loose_edges) {
              break;
            }

            loose_edges[num_loose_edges][0] = current_edge[0];
            loose_edges[num_loose_edges][1] = current_edge[1];

            num_loose_edges++;
          }
        }

        faces[i][0] = faces[num_faces-1][0];
        faces[i][1] = faces[num_faces-1][1];
        faces[i][2] = faces[num_faces-1][2];
        faces[i][3] = faces[num_faces-1][3];
        num_faces--;
        i--;
      }
    }
    
    for (auto i : std::views::iota(0u, num_loose_edges)) {
      if (num_faces >= max_faces) {
        break;
      }

      faces[num_faces][0] = loose_edges[i][0];
      faces[num_faces][1] = loose_edges[i][1];
      faces[num_faces][2] = p;
      faces[num_faces][3] = math::vector3::normalized(math::vector3::cross(loose_edges[i][0]-loose_edges[i][1], loose_edges[i][0]-p));
  
      if (math::vector3::dot(faces[num_faces][0], faces[num_faces][3]) + bias < 0.0f) {
        auto temp = faces[num_faces][0];
        faces[num_faces][0] = faces[num_faces][1];
        faces[num_faces][1] = temp;
        faces[num_faces][3] = -faces[num_faces][3];
      }

      num_faces++;
    }
  } 

  return faces[closest_face][3] * math::vector3::dot(faces[closest_face][0], faces[closest_face][3]);
}

auto gjk(const collider_data& first, const collider_data& second) -> std::optional<math::vector3> {
  static constexpr auto max_iterations = 64u;

  auto a = math::vector3{};
  auto b = math::vector3{};
  auto c = math::vector3{};
  auto d = math::vector3{};

  auto direction = first.position - second.position;

  c = support(direction, second) - support(-direction, first);

  direction = -c;
  
  b = support(direction, second) - support(-direction, first);

  if (math::vector3::dot(b, direction) <= 0.0f) {
    return std::nullopt;
  }

  direction = math::vector3::cross(math::vector3::cross(c - b, -b), c - b);

  if (direction == math::vector3::zero) {
    direction = math::vector3::cross(c - b, math::vector3::right);

    if (direction == math::vector3::zero) {
      direction = math::vector3::cross(c - b, math::vector3::forward);
    }
  }

  auto dimension = 2u;

  for (auto iteration : std::views::iota(0u, max_iterations)) {
    a = support(direction, second) - support(-direction, first);

    if (math::vector3::dot(a, direction) < 0.0f) {
      return std::nullopt;
    }

    dimension++;

    if (dimension == 3u) {
      update_simplex3(a, b, c, d, dimension, direction);
    } else if (update_simplex4(a, b, c, d, dimension, direction)) {
      return expanding_polytope(a, b, c, d, first, second);
    }
  }

  return std::nullopt;
}

} // namespace sbx::physics
