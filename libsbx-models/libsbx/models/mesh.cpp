#include <libsbx/models/mesh.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::models {

mesh::mesh(const std::filesystem::path& path)
: graphics::mesh<vertex3d>{} {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error{"Mesh file not found: " + path.string()};
  }

  const auto extension = path.extension().string();

  const auto entry = _loaders().find(extension);

  if (entry == _loaders().end()) {
    throw std::runtime_error{"No loader found for extension: " + extension};
  }

  auto& loader = entry->second;

  auto timer = utility::timer{};

  auto [vertices, indices, submeshes] = std::invoke(loader, path);

  _upload_vertices(std::move(vertices), std::move(indices));

  _submeshes = std::move(submeshes);

  for (const auto& submesh : _submeshes) {
    core::logger::debug("index_count: {}, index_offset: {}, vertex_offset: {}", submesh.index_count,  submesh.index_offset,  submesh.vertex_offset);
  }

  core::logger::debug("Loaded mesh: {}, vertices: {}, indices: {} in {} ms", path.string(), vertices.size(), indices.size(), units::quantity_cast<units::millisecond>(timer.elapsed()).value());
}

mesh::~mesh() {

}

} // namespace sbx::models
