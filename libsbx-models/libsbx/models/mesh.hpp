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

class mesh : public graphics::mesh<vertex3d> {

  using base = graphics::mesh<vertex3d>;

public:

  using mesh_data = base::mesh_data;

  using base::mesh;

  mesh(const std::filesystem::path& path, const std::uint32_t lod = 1u);

  ~mesh() override;

  auto lod() const noexcept -> std::uint32_t {
    return _lod;
  }

private:

  static auto _load(const std::filesystem::path& path, const std::uint32_t lod) -> mesh_data;

  // static auto _process(const std::filesystem::path& path, const mesh_data& data) -> void;

  std::uint32_t _lod;

}; // class mesh

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_HPP_
