#ifndef DEMO_TERRAIN_TERRAIN_MODULE_HPP_
#define DEMO_TERRAIN_TERRAIN_MODULE_HPP_

#include <libsbx/units/time.hpp>

#include <libsbx/math/uuid.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>

#include <demo/terrain/planet.hpp>

namespace demo {

class terrain_module final : public sbx::core::module<terrain_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<sbx::graphics::graphics_module, sbx::scenes::scenes_module>{});

public:

  terrain_module() {
    
  }

  ~terrain_module() override = default;

  auto load_terrain_in_scene(sbx::scenes::scene& scene) -> void {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    const auto chunk_size = sbx::math::vector2u{25u, 25u};

    _mesh_id = graphics_module.add_asset<sbx::models::mesh>(_generate_plane(chunk_size, sbx::math::vector2u{5u, 5u}));

    _texture_id = graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/grass/albedo.png");
    _normal_texture_id = graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/grass/normal.png");

    const auto grid = sbx::math::vector2{12.0f, 12.0f};

    const auto offset = sbx::math::vector2{chunk_size.x() * grid.x() * 0.5f, chunk_size.y() * grid.y() * 0.5f};

    _node = scene.create_node("Terrain");

    for (auto y = 0u; y < grid.y(); ++y) {
      for (auto x = 0u; x < grid.x(); ++x) {
        auto chunk = scene.create_child_node(_node, fmt::format("Chunk{}{}", x, y));

        auto submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};

        submeshes.emplace_back(sbx::scenes::static_mesh::submesh{
          .index = 0u,
          .tint = sbx::math::color::white(),
          .material = sbx::scenes::static_mesh::material{
            .metallic = 0.0f,
            .roughness = 1.0f,
            .flexibility = 0.0f,
            .anchor_height = 0.0f
          },
          .albedo_texture = _texture_id
        });

        scene.add_component<sbx::scenes::static_mesh>(chunk, _mesh_id, submeshes);

        const auto position = sbx::math::vector3{x * chunk_size.x() - offset.x(), 0.0f, y * chunk_size.y() - offset.y()};

        auto& transform = scene.get_component<sbx::math::transform>(chunk);

        transform.set_position(position);
      }
    }

    auto icosphere_tile_mesh = demo::icosphere_tile_mesh{4u, 0.02f};

    // _planet_id = graphics_module.add_asset<sbx::models::mesh>(std::make_unique<sbx::models::mesh>(icosphere_tile_mesh.get_vertices(), icosphere_tile_mesh.get_indices(), icosphere_tile_mesh.get_bounds()));
    _planet_id = graphics_module.add_asset<sbx::models::mesh>(_generate_icosphere(10.0f, 4u));

    auto planet = scene.create_node("Planet");
    auto planet_submeshes = std::vector<sbx::scenes::static_mesh::submesh>{};
    planet_submeshes.emplace_back(sbx::scenes::static_mesh::submesh{
      .index = 0u,
      .tint = sbx::math::color::white(),
      .material = sbx::scenes::static_mesh::material{
        .metallic = 0.0f,
        .roughness = 1.0f,
        .flexibility = 0.0f,
        .anchor_height = 0.0f
      },
      .albedo_texture = _texture_id
    });

    scene.add_component<sbx::scenes::static_mesh>(planet, _planet_id, planet_submeshes);

    auto& planet_transform = scene.get_component<sbx::math::transform>(planet);
    planet_transform.set_position(sbx::math::vector3{0.0f, 15.0f, 0.0f});
  }

  auto update() -> void override {

  }

private:

  // auto _generate_polygon(const std::vector<sbx::math::vector2>& points) -> std::unique_ptr<sbx::models::mesh> {
  //   auto vertices = std::vector<sbx::models::vertex3d>{};
  //   auto indices = std::vector<std::uint32_t>{};

  //   for (const auto& point : points) {
  //     vertices.emplace_back(sbx::math::vector3{point.x(), 0.0f, point.y()}, sbx::math::vector3::up, sbx::math::vector2{0.0f, 0.0f});
  //   }

  //   for (auto i = 0u; i < points.size() - 2u; ++i) {
  //     indices.emplace_back(0u);
  //     indices.emplace_back(i + 1u);
  //     indices.emplace_back(i + 2u);
  //   }

  //   return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices));
  // }

  static auto spherical_uv(const sbx::math::vector3& p) -> sbx::math::vector2 {
    const auto pi = std::numbers::pi_v<std::float_t>;
    const float u = 0.5f + std::atan2(p.z(), p.x()) / (2.0f * pi);
    const float v = 0.5f - std::asin(p.y()) / pi;
    return sbx::math::vector2{u, v};
  }

  auto _generate_plane(const sbx::math::vector2u& size, const sbx::math::vector2u& subdivisions) -> std::unique_ptr<sbx::models::mesh> {
    auto vertices = std::vector<sbx::models::vertex3d>{};
    auto indices = std::vector<std::uint32_t>{};

    const auto x_vertices = subdivisions.x() + 1u;
    const auto y_vertices = subdivisions.y() + 1u;

    const auto tile_size = sbx::math::vector2{static_cast<std::float_t>(size.x() / subdivisions.x()), static_cast<std::float_t>(size.y() / subdivisions.y())};
    
    const auto offset = sbx::math::vector2{static_cast<std::float_t>(size.x() / 2.0f), static_cast<std::float_t>(size.y() / 2.0f)};
    
    // Generate vertices
    for (auto y = 0u; y < subdivisions.y() + 1u; ++y) {
      for (auto x = 0u; x < subdivisions.x() + 1u; ++x) {
        const auto position = sbx::math::vector3{static_cast<std::float_t>(x * tile_size.x() - offset.x()), 0.0f, static_cast<std::float_t>(y * tile_size.y() - offset.y())};
        const auto normal = sbx::math::vector3::up;
        const auto tangent = sbx::math::vector4(sbx::math::vector3::right, 1.0f);
        const auto uv = sbx::math::vector2{static_cast<std::float_t>(x % 2), static_cast<std::float_t>(y % 2)};

        vertices.emplace_back(position, normal, tangent, uv);
      }
    }

    // Calculate indices

    const auto vertex_count = subdivisions.x() + 1u;

    for (auto i = 0u; i < vertex_count * vertex_count - vertex_count; ++i) {
      if ((i + 1u) % vertex_count == 0u) {
        continue;
      }

      indices.emplace_back(i);
      indices.emplace_back(i + vertex_count);
      indices.emplace_back(i + vertex_count + 1u);

      indices.emplace_back(i);
      indices.emplace_back(i + vertex_count + 1u);
      indices.emplace_back(i + 1u);
    }

    const auto half_width = static_cast<std::float_t>(size.x()) / 2.0f;
    const auto half_height = static_cast<std::float_t>(size.y()) / 2.0f;

    const auto min = sbx::math::vector3{-half_width, -0.1f, -half_height};
    const auto max = sbx::math::vector3{half_width, 0.1f, half_height};

    return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices), sbx::math::volume{min, max});
  }

  auto _generate_icosphere(float radius, std::uint32_t subdivisions) -> std::unique_ptr<sbx::models::mesh> {
    using vec3 = sbx::math::vector3;
    using vec2 = sbx::math::vector2;
    using vec4 = sbx::math::vector4;
    using triangle = std::array<std::uint32_t, 3>;

    // Create 12 vertices of an icosahedron
    const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

    std::vector<vec3> positions = {
      {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
      { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
      { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
    };

    for (auto& pos : positions) {
      pos = sbx::math::vector3::normalized(pos) * radius;
    }

    std::vector<triangle> faces = {
      {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
      {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
      {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
      {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
    };

    // Midpoint cache to avoid duplicates
    std::map<std::pair<std::uint32_t, std::uint32_t>, std::uint32_t> midpoint_cache;

    auto get_midpoint = [&](std::uint32_t i1, std::uint32_t i2) -> std::uint32_t {
      auto key = std::minmax(i1, i2);
      if (auto it = midpoint_cache.find(key); it != midpoint_cache.end()) {
        return it->second;
      }

      const auto& v1 = positions[i1];
      const auto& v2 = positions[i2];
      vec3 midpoint = sbx::math::vector3::normalized((v1 + v2) * 0.5f) * radius;

      positions.push_back(midpoint);
      std::uint32_t index = static_cast<std::uint32_t>(positions.size() - 1);
      midpoint_cache[key] = index;
      return index;
    };

    // Subdivide faces
    for (std::uint32_t i = 0; i < subdivisions; ++i) {
      std::vector<triangle> new_faces;
      for (const auto& [a, b, c] : faces) {
        std::uint32_t ab = get_midpoint(a, b);
        std::uint32_t bc = get_midpoint(b, c);
        std::uint32_t ca = get_midpoint(c, a);

        new_faces.push_back({a, ab, ca});
        new_faces.push_back({b, bc, ab});
        new_faces.push_back({c, ca, bc});
        new_faces.push_back({ab, bc, ca});
      }
      faces = std::move(new_faces);
    }

    // Convert to vertex3d format
    std::vector<sbx::models::vertex3d> vertices;
    vertices.reserve(positions.size());

    for (const auto& pos : positions) {
      vec3 normal = sbx::math::vector3::normalized(pos);
      vec4 tangent = vec4(1.0f, 0.0f, 0.0f, 1.0f); // Placeholder
      vec2 uv = vec2(0.0f); // Could be improved with spherical UVs

      vertices.emplace_back(pos, normal, tangent, uv);
    }

    // Flatten triangle indices
    std::vector<std::uint32_t> indices;
    indices.reserve(faces.size() * 3);
    for (const auto& [a, b, c] : faces) {
      indices.push_back(a);
      indices.push_back(b);
      indices.push_back(c);
    }

    // AABB bounds (approximate)
    vec3 bounds_min = vec3(-radius, -radius, -radius);
    vec3 bounds_max = vec3( radius,  radius,  radius);

    return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices), sbx::math::volume{bounds_min, bounds_max});
  }

  sbx::math::uuid _mesh_id;
  sbx::math::uuid _planet_id;
  sbx::graphics::image_handle _texture_id;
  sbx::graphics::image_handle _normal_texture_id;
  sbx::scenes::node _node;

}; // class terrain_module

} // namespace demo

#endif // DEMO_TERRAIN_TERRAIN_MODULE_HPP_
