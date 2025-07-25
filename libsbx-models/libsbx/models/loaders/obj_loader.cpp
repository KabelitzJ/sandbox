#include <libsbx/models/loaders/obj_loader.hpp>

#include <tiny_obj_loader.h>

#include <libsbx/utility/logger.hpp>

namespace sbx::models {

auto obj_loader::load(const std::filesystem::path& path) -> mesh::mesh_data {
  auto data = mesh::mesh_data{};

  // [NOTE] KAJ 2023-05-29 : Old tinyobjloader API.
  auto attributes = tinyobj::attrib_t{};
  auto shapes = std::vector<tinyobj::shape_t>{};
  auto materials = std::vector<tinyobj::material_t>{};
  auto error = std::string{};
  auto warning = std::string{};

  const auto result = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.string().c_str(), path.parent_path().string().c_str());

  if (!warning.empty()) {
    utility::logger<"models">::warn("{}", warning);
  }

  if (!error.empty()) {
    utility::logger<"models">::error("{}", error);
  }

  if (!result) {
    throw std::runtime_error{fmt::format("Failed to load mesh '{}'", path.string())};
  }

  auto unique_vertices = std::unordered_map<vertex3d, std::uint32_t>{};

  for (const auto& shape : shapes) {
    auto submesh = graphics::submesh{};

    submesh.index_offset = data.indices.size();

    // [NOTE] KAJ 2023-11-22 : This is a offset into the vertex buffer. We dont want to use this.
    submesh.vertex_offset = 0u;

    for (const auto& index : shape.mesh.indices) {
      auto vertex = vertex3d{};

      vertex.position.x() = attributes.vertices[3 * index.vertex_index + 0];
      vertex.position.y() = attributes.vertices[3 * index.vertex_index + 1];
      vertex.position.z() = attributes.vertices[3 * index.vertex_index + 2];

      vertex.normal.x() = attributes.normals[3 * index.normal_index + 0];
      vertex.normal.y() = attributes.normals[3 * index.normal_index + 1];
      vertex.normal.z() = attributes.normals[3 * index.normal_index + 2];

      vertex.uv.x() = attributes.texcoords[2 * index.texcoord_index + 0];
      vertex.uv.y() = attributes.texcoords[2 * index.texcoord_index + 1];

      if (auto entry = unique_vertices.find(vertex); entry != unique_vertices.end()) {
        data.indices.push_back(entry->second);
      } else {
        unique_vertices.insert({vertex, data.vertices.size()});
        data.indices.push_back(data.vertices.size());
        data.vertices.push_back(vertex);
      }
    }

    submesh.index_count = data.indices.size() - submesh.index_offset;

    data.submeshes.push_back(submesh);
  }

  return data;
}

} // namespace sbx::models
