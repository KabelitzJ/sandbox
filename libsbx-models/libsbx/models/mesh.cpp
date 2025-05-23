#include <libsbx/models/mesh.hpp>

#include <libsbx/units/bytes.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::models {

mesh::mesh(const std::filesystem::path& path)
: base{_load(path)} { }

mesh::~mesh() {

}

auto mesh::_load(const std::filesystem::path& path) -> mesh_data {
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

  auto data = std::invoke(load, path);

  const auto vertices_count = data.vertices.size();
  const auto indices_count = data.indices.size();

  const auto b = units::byte{vertices_count * sizeof(vertex3d)};

  const auto kb = units::quantity_cast<units::kilobyte>(b);

  utility::logger<"models">::debug("Loaded mesh: {}, vertices: {}, indices: {}, size: {} kb in {:.2f}ms", path.string(), vertices_count, indices_count, kb.value(), units::quantity_cast<units::millisecond>(timer.elapsed()).value());

  return data;
}

} // namespace sbx::models
