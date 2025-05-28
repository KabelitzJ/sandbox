#ifndef DEMO_TERRAIN_TERRAIN_MODULE_HPP_
#define DEMO_TERRAIN_TERRAIN_MODULE_HPP_

#include <libsbx/units/time.hpp>

#include <libsbx/math/uuid.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/camera.hpp>
#include <libsbx/scenes/components/static_mesh.hpp>

// #include <demo/terrain/voronoi.hpp> 

namespace demo {

class terrain_module final : public sbx::core::module<terrain_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<sbx::graphics::graphics_module, sbx::scenes::scenes_module>{});

public:

  terrain_module() {
    
  }

  ~terrain_module() override = default;

  auto load_terrain_in_scene(sbx::scenes::scene& scene) -> void {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    const auto chunk_size = sbx::math::vector2u{20u, 20u};

    _mesh_id = graphics_module.add_asset<sbx::models::mesh>(_generate_plane(chunk_size, sbx::math::vector2u{5u, 5u}));

    _texture_id = graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/grass/albedo.png");
    _normal_texture_id = graphics_module.add_resource<sbx::graphics::image2d>("demo/assets/textures/grass/normal.png");

    const auto grid = sbx::math::vector2{15.0f, 15.0f};

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

        // chunk.add_component<sbx::scenes::collider>(sbx::scenes::aabb_collider{sbx::math::vector3{-chunk_size.x() / 2.0f, 0.0f, -chunk_size.y() / 2.0f}, sbx::math::vector3{chunk_size.x() / 2.0f, 1.0f, chunk_size.y() / 2.0f}});

        auto& transform = scene.get_component<sbx::math::transform>(chunk);

        transform.set_position(position);
      }
    }
  }

  auto update() -> void override {

  }

private:

  auto _generate_polygon(const std::vector<sbx::math::vector2>& points) -> std::unique_ptr<sbx::models::mesh> {
    auto vertices = std::vector<sbx::models::vertex3d>{};
    auto indices = std::vector<std::uint32_t>{};

    for (const auto& point : points) {
      vertices.emplace_back(sbx::math::vector3{point.x(), 0.0f, point.y()}, sbx::math::vector3::up, sbx::math::vector2{0.0f, 0.0f});
    }

    for (auto i = 0u; i < points.size() - 2u; ++i) {
      indices.emplace_back(0u);
      indices.emplace_back(i + 1u);
      indices.emplace_back(i + 2u);
    }

    return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices));
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
        const auto uv = sbx::math::vector2{static_cast<std::float_t>(x % 2), static_cast<std::float_t>(y % 2)};

        vertices.emplace_back(position, normal, uv);
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

    return std::make_unique<sbx::models::mesh>(std::move(vertices), std::move(indices));
  }

  sbx::math::uuid _mesh_id;
  sbx::graphics::image_handle _texture_id;
  sbx::graphics::image_handle _normal_texture_id;
  sbx::scenes::node _node;

}; // class terrain_module

} // namespace demo

#endif // DEMO_TERRAIN_TERRAIN_MODULE_HPP_
