#ifndef LIBSBX_MODELS_LOADERS_GLTF_LOADER_HPP_
#define LIBSBX_MODELS_LOADERS_GLTF_LOADER_HPP_

#include <libsbx/models/mesh.hpp>

namespace sbx::models {

class gltf_loader : public mesh::loader<gltf_loader> {

  inline static const auto is_registered = register_extensions(".gltf");

public:

  static auto load(const std::filesystem::path& path) -> mesh_data;

}; // class gltf_loader

} // namespace sbx::models

#endif // LIBSBX_MODELS_LOADERS_GLTF_LOADER_HPP_
