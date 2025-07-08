#include <libsbx/animation/loaders/gltf_loader.hpp>

#include <filesystem>
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

#include <libsbx/animation/vertex3d.hpp>

namespace sbx::animation {

static auto _convert_vec2(const aiVector2D& vector) -> math::vector2 {
  return {vector.x, vector.y};
}

static auto _convert_vec3(const aiVector3D& vector) -> math::vector3 {
  return {vector.x, vector.y, vector.z};
}

static auto _load_mesh(const aiMesh* mesh, mesh::mesh_data& data) -> void {
  if (!mesh->HasNormals()) {
    throw std::runtime_error{fmt::format("Mesh '{}' does not have normals", mesh->mName.C_Str())};
  }

  if (!mesh->HasTextureCoords(0)) {
    throw std::runtime_error{fmt::format("Mesh '{}' does not have uvs", mesh->mName.C_Str())};
  }

  if (!mesh->HasTangentsAndBitangents()) {
    throw std::runtime_error{fmt::format("Mesh '{}' does not have tangents", mesh->mName.C_Str())};
  }

  if (!mesh->HasBones()) {
    throw std::runtime_error{fmt::format("Mesh '{}' does not have bones", mesh->mName.C_Str())};
  }

  auto submesh = graphics::submesh{};
  submesh.vertex_offset = 0u;
  submesh.index_offset = data.indices.size();

  submesh.bounds = math::volume{_convert_vec3(mesh->mAABB.mMin), _convert_vec3(mesh->mAABB.mMax)};

  const auto vertices_count = data.vertices.size();

  data.vertices.reserve(vertices_count + mesh->mNumVertices);

  for (auto i = 0u; i < mesh->mNumVertices; ++i) {
    auto vertex = animation::vertex3d{};
    vertex.position = _convert_vec3(mesh->mVertices[i]);
    vertex.normal = _convert_vec3(mesh->mNormals[i]);
    vertex.uv = _convert_vec3(mesh->mTextureCoords[0][i]);
    vertex.tangent = _convert_vec3(mesh->mTangents[i]);
    
    for (auto bone = 0u; bone < std::min(mesh->mNumBones, 4u); ++bone) {
      vertex.bone_ids[i] = mesh->mBones[i]->mWeights[bone].mVertexId;
      vertex.bone_weights[i] = mesh->mBones[i]->mWeights[bone].mWeight;
    }

    data.vertices.push_back(vertex);
  }

  data.indices.reserve(data.indices.size() + mesh->mNumFaces * 3u);

  // [NOTE] KAJ 2025-07-08 : We need to add vertices_count since all submeshes are stored in one vertex buffer
  for (auto i = 0u; i < mesh->mNumFaces; ++i) {
    data.indices.push_back(vertices_count + mesh->mFaces[i].mIndices[0]);
    data.indices.push_back(vertices_count + mesh->mFaces[i].mIndices[1]);
    data.indices.push_back(vertices_count + mesh->mFaces[i].mIndices[2]);
  }

  submesh.index_count = data.indices.size() - submesh.index_offset;

  data.submeshes.push_back(submesh);
}

static auto _load_node(const aiNode* node, const aiScene* scene, mesh::mesh_data& data) -> void {
  for (auto i = 0u; i < node->mNumMeshes; ++i) {
    _load_mesh(scene->mMeshes[node->mMeshes[i]], data);
  }

  for (auto i = 0u; i < node->mNumChildren; ++i) {
    _load_node(node->mChildren[i], scene, data);
  }
}

auto gltf_loader::load(const std::filesystem::path& path) -> mesh::mesh_data {
  auto data = mesh::mesh_data{};

  static constexpr auto import_flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality;

  auto importer = Assimp::Importer{};

  const auto* scene = importer.ReadFile(path.string(), import_flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    throw std::runtime_error{fmt::format("Error loading mesh '{}': {}", path.string(), importer.GetErrorString())};
  }

  _load_node(scene->mRootNode, scene, data);

  return data;
}

} // namespace sbx::animation
