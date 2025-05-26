#include <libsbx/models/loaders/sbxmsh_loader.hpp>

#include <filesystem>
#include <fstream>

#include <fmt/format.h>

#include <libsbx/utility/logger.hpp>

#include <libsbx/math/volume.hpp>
#include <libsbx/math/sphere.hpp>

namespace sbx::models {

auto sbxmsh_loader::load(const std::filesystem::path& path) -> mesh::mesh_data {
  auto data = mesh::mesh_data{};

  if (!std::filesystem::exists(path)) {
    throw std::runtime_error{fmt::format("Mesh file not found: {}", path.string())};
  }

  auto input_file = std::ifstream{path, std::ios::binary};

  if (!input_file.is_open()) {
    throw std::runtime_error{fmt::format("Failed to open mesh file: {}", path.string())};
  }

  auto header = mesh::file_header{};

  input_file.read(reinterpret_cast<char*>(&header), sizeof(mesh::file_header));

  if (header.magic != 69u) {
    throw std::runtime_error{fmt::format("Invalid magic number in mesh file: {}", path.string())};
  }

  if (header.version != 1u) {
    throw std::runtime_error{fmt::format("Unsupported mesh file version: {} in file: {}", header.version, path.string())};
  }

  data.indices.resize(header.index_count);
  input_file.read(reinterpret_cast<char*>(data.indices.data()), header.index_count * header.index_type_size);

  data.vertices.resize(header.vertex_count);
  input_file.read(reinterpret_cast<char*>(data.vertices.data()), header.vertex_count * header.vertex_type_size);

  data.submeshes.resize(header.submesh_count);
  input_file.read(reinterpret_cast<char*>(data.submeshes.data()), header.submesh_count * sizeof(graphics::submesh));

  auto aabb = math::volume{math::vector3{}, math::vector3{}};
  input_file.read(reinterpret_cast<char*>(&aabb), sizeof(math::volume));

  auto sphere = math::sphere{math::vector3{}, 0.0f};
  input_file.read(reinterpret_cast<char*>(&sphere), sizeof(math::sphere));

  input_file.close();

  return data;
}

} // namespace sbx::models
