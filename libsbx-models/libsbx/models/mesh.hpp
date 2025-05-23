#ifndef LIBSBX_MODELS_MESH_HPP_
#define LIBSBX_MODELS_MESH_HPP_

#include <filesystem>

#include <libsbx/utility/hash.hpp>

#include <libsbx/io/loader_factory.hpp>

#include <libsbx/graphics/pipeline/mesh.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

class mesh : public graphics::mesh<vertex3d>, public io::loader_factory<mesh, graphics::mesh<vertex3d>::mesh_data> {

  using base = graphics::mesh<vertex3d>;

public:

  using mesh_data = graphics::mesh<vertex3d>::mesh_data;

  using base::mesh;

  mesh(const std::filesystem::path& path);

  ~mesh() override;

private:

  auto _load(const std::filesystem::path& path) -> mesh_data;

}; // class mesh

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_HPP_
