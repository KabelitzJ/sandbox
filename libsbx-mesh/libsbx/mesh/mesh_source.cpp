#include <libsbx/mesh/mesh_source.hpp>

#include <fstream>
#include <bit>

#include <fmt/format.h>

#include <libbase64.h>

#include <nlohmann/json.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <libsbx/io/read_file.hpp>

#include <libsbx/math/vector2.hpp>  
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/quaternion.hpp>

#include <libsbx/graphics/pipeline/mesh.hpp>

namespace sbx::mesh {

static auto aivec2_to_vector2(const aiVector2D& from) -> math::vector2 {
  return {from.x, from.y};
}

static auto aivec3_to_vector3(const aiVector3D& from) -> math::vector3 {
  return {from.x, from.y, from.z};
}

mesh_source::mesh_source(const std::filesystem::path& path) {
  auto file = std::ifstream{path};

  auto importer = Assimp::Importer{};

  const auto* scene = importer.ReadFile(
    path.string(),
    aiProcess_Triangulate |
    aiProcess_GenNormals |
    aiProcess_JoinIdenticalVertices |
    aiProcess_LimitBoneWeights |
    aiProcess_ImproveCacheLocality |
    aiProcess_CalcTangentSpace |
    aiProcess_ValidateDataStructure
  );

  if (!scene || !scene->HasMeshes()) {
    throw std::runtime_error("Failed to load mesh from " + path.string());
  }

  auto submesh = graphics::submesh{};
  submesh.vertex_offset = 0u;
  
  for (auto i = 0u; i < scene->mNumMeshes; ++i) {
    const auto* mesh = scene->mMeshes[i];

    submesh.index_offset = _indices.size();

    const auto& bounds = mesh->mAABB;

    submesh.bounds = math::volume{aivec3_to_vector3(bounds.mMin), aivec3_to_vector3(bounds.mMax)};

    if (mesh->HasNormals()) {
      _normals.reserve(_normals.size() + mesh->mNumVertices);
    }

    if (mesh->HasTextureCoords(0)) {
      _uvs.reserve(_uvs.size() + mesh->mNumVertices);
    }

    if (mesh->HasTangentsAndBitangents()) {
      _tangents.reserve(_tangents.size() + mesh->mNumVertices);
      _bitangents.reserve(_bitangents.size() + mesh->mNumVertices);
    }

    for (auto j = 0u; j < mesh->mNumVertices; ++j) {
      _positions.push_back(aivec3_to_vector3(mesh->mVertices[j]));

      if (mesh->HasNormals()) {
        _normals.push_back(aivec3_to_vector3(mesh->mNormals[j]));
      }

      if (mesh->HasTextureCoords(0)) {
        _uvs.push_back(aivec3_to_vector3(mesh->mTextureCoords[0][j]));
      }

      if (mesh->HasTangentsAndBitangents()) {
        _tangents.push_back(aivec3_to_vector3(mesh->mTangents[j]));
        _bitangents.push_back(aivec3_to_vector3(mesh->mBitangents[j]));
      }
    }

    const auto old_index_count = _indices.size();

    for (auto j = 0u; j < mesh->mNumFaces; ++j) {
      const auto& face = mesh->mFaces[j];

      for (auto k = 0u; k < face.mNumIndices; ++k) {
        _indices.push_back(face.mIndices[k]);
      }
    }

    submesh.index_count = static_cast<std::uint32_t>(_indices.size() - old_index_count);

    _submeshes.push_back(submesh);
  }

}

} // namespace sbx::mesh
