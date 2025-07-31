#ifndef LIBSBX_MODELS_MESH_HPP_
#define LIBSBX_MODELS_MESH_HPP_

#include <filesystem>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/volume.hpp>
#include <libsbx/math/sphere.hpp>

#include <libsbx/io/loader_factory.hpp>

#include <libsbx/graphics/pipeline/mesh.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

template<std::uint32_t LOD = 1u>
requires (LOD >= 1u)
class lod_mesh : public graphics::mesh<vertex3d, LOD> {

  using base = graphics::mesh<vertex3d, LOD>;

public:

  using mesh_data = base::mesh_data;

  using base::mesh;

  lod_mesh(const std::filesystem::path& path);

  ~lod_mesh() override;

private:

  static auto _load(const std::filesystem::path& path) -> mesh_data;

  // static auto _process(const std::filesystem::path& path, const mesh_data& data) -> void;

}; // class mesh

using mesh = lod_mesh<1u>;

} // namespace sbx::models

#include <libsbx/models/mesh.ipp>

#endif // LIBSBX_MODELS_MESH_HPP_
