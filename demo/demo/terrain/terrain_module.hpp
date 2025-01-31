#ifndef DEMO_TERRAIN_TERRAIN_MODULE_HPP_
#define DEMO_TERRAIN_TERRAIN_MODULE_HPP_

#include <libsbx/units/time.hpp>

#include <libsbx/math/uuid.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <demo/terrain/voronoi.hpp> 

namespace demo {

class terrain_module final : public sbx::core::module<terrain_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<sbx::graphics::graphics_module, sbx::scenes::scenes_module>{});

public:

  terrain_module() {
    
  }

  ~terrain_module() override = default;

  auto load_terrain_in_scene(sbx::scenes::scene& scene) -> void {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    const auto chunk_size = sbx::math::vector2u{50u, 50u};

    _mesh_id = graphics_module.add_asset<sbx::models::mesh>(_generate_plane(chunk_size, sbx::math::vector2u{10u, 10u}));

    _texture_id = graphics_module.add_asset<sbx::graphics::image2d>("demo/assets/textures/prototype_black.png");

    const auto grid = sbx::math::vector2{10.0f, 10.0f};

    const auto offset = sbx::math::vector2{chunk_size.x() * grid.x() * 0.5f, chunk_size.y() * grid.y() * 0.5f};

    auto node = scene.create_node("Terrain");

    _node_id = node.get_component<sbx::scenes::id>();

    for (auto y = 0u; y < grid.y(); ++y) {
      for (auto x = 0u; x < grid.x(); ++x) {
        auto chunk = scene.create_child_node(node, fmt::format("Chunk{}{}", x, y));

        chunk.add_component<sbx::scenes::static_mesh>(_mesh_id, sbx::math::color::white, sbx::scenes::static_mesh::material{0.0f, 1.0f, 0.0f, 0.0f}, _texture_id);

        const auto position = sbx::math::vector3{x * chunk_size.x() - offset.x(), 0.0f, y * chunk_size.y() - offset.y()};

        auto& transform = chunk.get_component<sbx::math::transform>();

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

  auto _generate_plane(const sbx::math::vector2u& size, const sbx::math::vector2u& tile_size) -> std::unique_ptr<sbx::models::mesh> {
    auto vertices = std::vector<sbx::models::vertex3d>{};
    auto indices = std::vector<std::uint32_t>{};

    // Generate vertices

    const auto tile_count = sbx::math::vector2u{size.x() / tile_size.x(), size.y() / tile_size.y()};

    const auto offset = sbx::math::vector2{static_cast<std::float_t>(size.x() / 2.0f), static_cast<std::float_t>(size.y() / 2.0f)};

    for (auto y = 0u; y < tile_count.y() + 1u; ++y) {
      for (auto x = 0u; x < tile_count.x() + 1u; ++x) {
        const auto position = sbx::math::vector3{static_cast<std::float_t>(x * tile_size.x() - offset.x()), 0.0f, static_cast<std::float_t>(y * tile_size.y() - offset.y())};
        const auto normal = sbx::math::vector3::up;
        const auto uv = sbx::math::vector2{static_cast<std::float_t>(x % 2), static_cast<std::float_t>(y % 2)};

        vertices.emplace_back(position, normal, uv);
      }
    }

    // Calculate indices

    const auto vertex_count = tile_count.x() + 1u;

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
  sbx::math::uuid _texture_id;
  sbx::math::uuid _node_id;

}; // class terrain_module

} // namespace demo

#endif // DEMO_TERRAIN_TERRAIN_MODULE_HPP_
