#ifndef DEMO_TERRAIN_PLANET_HPP_
#define DEMO_TERRAIN_PLANET_HPP_

#include <numbers>
#include <limits>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace demo {

class icosphere_tile_mesh {

public:

  using vec2 = sbx::math::vector2;
  using vec3 = sbx::math::vector3;
  using vec4 = sbx::math::vector4;

  icosphere_tile_mesh(std::uint32_t subdivisions, std::float_t scattering) {
    generate_icosahedron(subdivisions, scattering);
  }

  [[nodiscard]] const std::vector<sbx::models::vertex3d>& get_vertices() const { return _vertices; }
  [[nodiscard]] const std::vector<std::uint32_t>& get_indices() const { return _indices; }
  [[nodiscard]] const sbx::math::volume& get_bounds() const { return _bounds; }

private:

  std::vector<sbx::models::vertex3d> _vertices;
  std::vector<std::uint32_t> _indices;
  sbx::math::volume _bounds;

  static vec2 spherical_uv(const vec3& p) {
    const auto pi = std::numbers::pi_v<std::float_t>;
    const float u = 0.5f + std::atan2(p.z(), p.x()) / (2.0f * pi);
    const float v = 0.5f - std::asin(p.y()) / pi;
    return vec2{u, v};
  }

  void generate_icosahedron(std::uint32_t subdivisions, std::float_t scattering) {
    const float phi = std::numbers::phi_v<std::float_t>;

    std::vector<vec3> positions = {
      vec3::normalized(vec3{-1,  phi, 0}), vec3::normalized(vec3{ 1,  phi, 0}),
      vec3::normalized(vec3{-1, -phi, 0}), vec3::normalized(vec3{ 1, -phi, 0}),
      vec3::normalized(vec3{0, -1,  phi}), vec3::normalized(vec3{0,  1,  phi}),
      vec3::normalized(vec3{0, -1, -phi}), vec3::normalized(vec3{0,  1, -phi}),
      vec3::normalized(vec3{ phi, 0, -1}), vec3::normalized(vec3{ phi, 0,  1}),
      vec3::normalized(vec3{-phi, 0, -1}), vec3::normalized(vec3{-phi, 0,  1}),
    };

    _vertices.reserve(positions.size());
    for (const auto& pos : positions) {
      const vec3 normal = pos;
      const vec2 uv = spherical_uv(pos);
      const vec3 tangent = vec3::normalized(vec3::cross(vec3::up, normal));
      _vertices.emplace_back(pos, normal, vec4{tangent, 1.0f}, uv);
    }

    _indices = {
      0,11,5,  0,5,1,  0,1,7,  0,7,10,  0,10,11,
      1,5,9,  5,11,4, 11,10,2, 10,7,6,  7,1,8,
      3,9,4,  3,4,2,  3,2,6,  3,6,8,  3,8,9,
      4,9,5,  2,4,11, 6,2,10, 8,6,7,  9,8,1
    };

    vec3 min{std::numeric_limits<float>::max()};
    vec3 max{std::numeric_limits<float>::lowest()};
    for (const auto& v : _vertices) {
      min = vec3::min(min, v.position);
      max = vec3::max(max, v.position);
    }

    _bounds = sbx::math::volume{min, max};
  }

}; // class icosphere_tile_mesh

} // namespace demo

#endif // DEMO_TERRAIN_PLANET_HPP_
