#include <libsbx/models/loaders/fbx_loader.hpp>

#include <fstream>

namespace sbx::models {

auto fbx_loader::load(const std::filesystem::path& path) -> mesh::mesh_data {
  auto data = mesh::mesh_data{};

  return data;
}

} // namespace sbx::models
