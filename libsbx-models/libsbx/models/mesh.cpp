#include <libsbx/models/mesh.hpp>

#include <libsbx/units/bytes.hpp>

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

  auto& [load, unload] = entry->second;

  auto timer = utility::timer{};

  auto [vertices, indices, submeshes] = std::invoke(load, path);

  const auto vertices_count = vertices.size();
  const auto indices_count = vertices.size();

  _upload_vertices(std::move(vertices), std::move(indices));

  _submeshes = std::move(submeshes);

  const auto b = units::byte{vertices_count * sizeof(vertex3d)};

  const auto kb = units::quantity_cast<units::kilobyte>(b);

  core::logger::debug("Loaded mesh: {}, vertices: {}, indices: {}, size: {} kb in {:.2f}ms", path.string(), vertices_count, indices_count, kb.value(), units::quantity_cast<units::millisecond>(timer.elapsed()).value());
}

mesh::~mesh() {

}

} // namespace sbx::models
