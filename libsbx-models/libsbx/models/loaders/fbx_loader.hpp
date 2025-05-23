#ifndef LIBSBX_MODELS_LOADERS_FBX_LOADER_HPP_
#define LIBSBX_MODELS_LOADERS_FBX_LOADER_HPP_

#include <filesystem>

#include <libsbx/models/mesh.hpp>

namespace sbx::models {

class fbx_loader : public mesh::loader<fbx_loader> {

  inline static const auto is_registered = register_extensions(".fbx");

public:

  static auto load(const std::filesystem::path& path) -> mesh::mesh_data;

}; // class fbx_loader

} // namespace sbx::models

#endif // LIBSBX_MODELS_LOADERS_FBX_LOADER_HPP_
