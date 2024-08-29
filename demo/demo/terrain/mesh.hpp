#ifndef DEMO_TERRAIN_MESH_HPP_
#define DEMO_TERRAIN_MESH_HPP_

#include <libsbx/graphics/pipeline/mesh.hpp>

#include <demo/terrain/vertex.hpp>

namespace demo {

class mesh final : public sbx::graphics::mesh<vertex> {

  using base = sbx::graphics::mesh<vertex>;

public:

  using vertex_type = base::vertex_type;
  using index_type = base::index_type;

  mesh(std::vector<vertex_type>&& vertices, std::vector<index_type>&& indices, std::vector<std::float_t>&& heights, const sbx::math::vector2u& size)
  : base{std::move(vertices), std::move(indices)},
    _heights{std::move(heights)},
    _size{size} { }

  ~mesh() override = default;

  auto heights() const -> const std::vector<std::float_t>& {
    return _heights;
  }

  auto size() const -> const sbx::math::vector2u& {
    return _size;
  }

private:

  std::vector<std::float_t> _heights;
  sbx::math::vector2u _size;

}; // class mesh

} // namespace demo

#endif // DEMO_TERRAIN_MESH_HPP_
