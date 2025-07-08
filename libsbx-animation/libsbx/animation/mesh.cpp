#include <libsbx/animation/mesh.hpp>

#include <filesystem>
#include <fstream>
#include <cstdio>

#include <fmt/format.h>

#include <libsbx/units/bytes.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::animation {

mesh::mesh(const std::filesystem::path& path)
: base{_load(path)} { }

mesh::~mesh() {

}

auto mesh::_load(const std::filesystem::path& path) -> mesh_data {
  // [TODO] KAJ 2025-05-26 : Clean this up with two dedicated functions for loading and processing.
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error{"Mesh file not found: " + path.string()};
  }

  // const auto needs_processing = !std::filesystem::exists(std::filesystem::path{path}.replace_extension(".sbxmsh"));

  // const auto actual_path = needs_processing ? path : std::filesystem::path{path}.replace_extension(".sbxmsh"); 
  const auto actual_path = path;

  const auto extension = actual_path.extension().string();

  const auto entry = _loaders().find(extension);

  if (entry == _loaders().end()) {
    throw std::runtime_error{fmt::format("No loader registered for extension '{}'", extension)};
  }

  auto& [load, unload] = entry->second;

  auto timer = utility::timer{};

  auto data = std::invoke(load, actual_path);

  // if (needs_processing) {
  //   _process(actual_path, data);
  // }

  const auto vertices_count = data.vertices.size();
  const auto indices_count = data.indices.size();

  const auto b = units::byte{vertices_count * sizeof(vertex3d)};

  const auto kb = units::quantity_cast<units::kilobyte>(b);

  utility::logger<"models">::debug("Loaded mesh: {}, vertices: {}, indices: {}, size: {} kb in {:.2f}ms", actual_path.string(), vertices_count, indices_count, kb.value(), units::quantity_cast<units::millisecond>(timer.elapsed()).value());

  return data;
}

static auto calculate_aabb(const std::vector<vertex3d>& vertices) -> math::volume {
  if (vertices.empty()) {
    utility::logger<"models">::warn("Calculating AABB for empty mesh, returning default volume");
    return math::volume{sbx::math::vector3{0.0f, 0.0f, 0.0f}, sbx::math::vector3{0.0f, 0.0f, 0.0f}};
  }

  auto min = vertices.front().position;
  auto max = vertices.front().position;

  for (const auto& vertex : vertices) {
    min = math::vector3::min(min, vertex.position);
    max = math::vector3::max(max, vertex.position);
  }

  return math::volume{min, max};
}

static auto calculate_sphere(const std::vector<vertex3d>& vertices) -> math::sphere {
  if (vertices.empty()) {
    utility::logger<"models">::warn("Calculating sphere for empty mesh, returning default sphere");
    return math::sphere{sbx::math::vector3{0.0f, 0.0f, 0.0f}, 0.0f};
  }

  auto center = sbx::math::vector3{0.0f, 0.0f, 0.0f};

  for (const auto& vertex : vertices) {
    center += vertex.position;
  }

  center /= static_cast<float>(vertices.size());

  auto radius = 0.0f;

  for (const auto& vertex : vertices) {
    const auto distance = math::vector3::distance(center, vertex.position);
    radius = std::max(radius, std::abs(distance));
  }

  return math::sphere{center, radius};
}

auto mesh::_process(const std::filesystem::path& path, const mesh_data& data) -> void {
  const auto output_path = std::filesystem::path{path}.replace_extension(".sbxmsh");

  auto output_file = std::ofstream{output_path, std::ios::binary};

  if (!output_file.is_open()) {
    throw std::runtime_error{fmt::format("Failed to open output file: {}", output_path.string())};
  }

  auto header = file_header{};
  header.magic = 69u;
  header.version = 1u;
  header.index_type_size = sizeof(std::uint32_t);
  header.index_count = static_cast<std::uint32_t>(data.indices.size());
  header.vertex_type_size = sizeof(vertex3d);
  header.vertex_count = static_cast<std::uint32_t>(data.vertices.size());
  header.submesh_count = static_cast<std::uint32_t>(data.submeshes.size());

  output_file.write(reinterpret_cast<const char*>(&header), sizeof(file_header));

  output_file.write(reinterpret_cast<const char*>(data.indices.data()), data.indices.size() * sizeof(std::uint32_t));
  output_file.write(reinterpret_cast<const char*>(data.vertices.data()), data.vertices.size() * sizeof(vertex3d));
  output_file.write(reinterpret_cast<const char*>(data.submeshes.data()), data.submeshes.size() * sizeof(graphics::submesh));

  const auto aabb = calculate_aabb(data.vertices);
  output_file.write(reinterpret_cast<const char*>(&aabb), sizeof(math::volume));

  const auto sphere = calculate_sphere(data.vertices);
  output_file.write(reinterpret_cast<const char*>(&sphere), sizeof(math::sphere));

  utility::logger<"models">::debug("Processed mesh file '{}' to .sbxmsh format", output_path.string());

  output_file.flush();
  output_file.close();
}

} // namespace sbx::animation
