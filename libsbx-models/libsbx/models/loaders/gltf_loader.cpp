#include <libsbx/models/loaders/gltf_loader.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

namespace sbx::models {

auto gltf_loader::load(const std::filesystem::path& path) -> mesh_data {
  auto data = mesh_data{};

  return data;
}

} // namespace sbx::models
