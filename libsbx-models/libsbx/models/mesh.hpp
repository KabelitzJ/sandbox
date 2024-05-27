#ifndef LIBSBX_MODELS_MESH_HPP_
#define LIBSBX_MODELS_MESH_HPP_

#include <filesystem>

#include <libsbx/utility/hash.hpp>

#include <libsbx/io/loader_factory.hpp>

#include <libsbx/graphics/pipeline/mesh.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

struct mesh_data {
  std::vector<vertex3d> vertices;
  std::vector<std::uint32_t> indices;
  std::vector<graphics::submesh> submeshes;
}; // struct mesh_data

class mesh : public graphics::mesh<vertex3d>, public io::loader_factory<mesh, mesh_data> {

public:

  using graphics::mesh<vertex3d>::mesh;

  mesh(const std::filesystem::path& path);

  ~mesh() override;

}; // class mesh

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_HPP_
