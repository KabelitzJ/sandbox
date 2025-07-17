#include <libsbx/animations/mesh.hpp>

#include <filesystem>
#include <fstream>
#include <cstdio>

#include <fmt/format.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <libsbx/units/bytes.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/matrix4x4.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/utility/timer.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::animations {

static auto loaded_skeleton = animations::skeleton{};

mesh::mesh(const std::filesystem::path& path)
: base{_load(path)},
  _skeleton{std::move(loaded_skeleton)} { }

mesh::~mesh() {

}

static auto _convert_vec2(const aiVector2D& vector) -> math::vector2 {
  return math::vector2{vector.x, vector.y};
}

static auto _convert_vec3(const aiVector3D& vector) -> math::vector3 {
  return math::vector3{vector.x, vector.y, vector.z};
}

static auto _convert_mat4(const aiMatrix4x4& matrix) -> math::matrix4x4 {
  auto result = math::matrix4x4{};

  //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
  result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
  result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
  result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
  result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;

  return result;
}

using bone_map = std::unordered_map<std::string, std::uint32_t>;
using bone_offsets = std::vector<math::matrix4x4>;
using transform_map = std::unordered_map<std::string, math::matrix4x4>;

static auto _load_mesh(const aiMesh* mesh, mesh::mesh_data& data, bone_map& bone_map, bone_offsets& bone_offsets) -> void {
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

  data.vertices.reserve(data.vertices.size() + mesh->mNumVertices);
  data.indices.reserve(data.indices.size() + mesh->mNumFaces * 3);

  for (auto i = 0u; i < mesh->mNumVertices; ++i) {
    auto vertex = vertex3d{};
    vertex.position = _convert_vec3(mesh->mVertices[i]);
    vertex.normal = _convert_vec3(mesh->mNormals[i]);
    vertex.uv = _convert_vec3(mesh->mTextureCoords[0][i]);
    vertex.tangent = _convert_vec3(mesh->mTangents[i]);

    for (auto j = 0; j < 4; ++j) {
      vertex.bone_ids[j] = 0;
      vertex.bone_weights[j] = 0.0f;
    }

    data.vertices.push_back(vertex);
  }

  for (auto i = 0u; i < mesh->mNumFaces; ++i) {
    data.indices.push_back(vertices_count + mesh->mFaces[i].mIndices[0]);
    data.indices.push_back(vertices_count + mesh->mFaces[i].mIndices[1]);
    data.indices.push_back(vertices_count + mesh->mFaces[i].mIndices[2]);
  }

  submesh.index_count = data.indices.size() - submesh.index_offset;

  data.submeshes.push_back(submesh);

  for (auto i = 0u; i < mesh->mNumBones; ++i) {
    const auto* bone = mesh->mBones[i];
    const auto bone_name = std::string{bone->mName.C_Str()};

    if (bone_map.find(bone_name) == bone_map.end()) {
      auto bone_id = static_cast<std::uint32_t>(bone_offsets.size());
      bone_map.emplace(bone_name, bone_id);
      bone_offsets.push_back(_convert_mat4(bone->mOffsetMatrix));
    }
  }
}

static auto _load_node(const aiNode* node, const aiScene* scene, mesh::mesh_data& data, bone_map& bone_map, bone_offsets& bone_offsets) -> void {
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    const auto* mesh = scene->mMeshes[node->mMeshes[i]];
    _load_mesh(mesh, data, bone_map, bone_offsets);
  }

  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    _load_node(node->mChildren[i], scene, data, bone_map, bone_offsets);
  }
}

/**
 * @note bone_map and bone_offsets need to be populated by calling _load_node(scene->mRootNode, scene, data, bone_map, bone_offsets) before calling this function!
 */
static auto _build_skeleton_hierarchy(const aiNode* node, const std::string& parent_name, const bone_map& bone_map, const bone_offsets& bone_offsets, animations::skeleton& skeleton) -> void {
  const auto node_name = std::string{node->mName.C_Str()};

  const auto is_bone = bone_map.contains(node_name);
  auto parent_id = animations::skeleton::bone::null;

  if (is_bone) {
    if (!parent_name.empty() && bone_map.contains(parent_name)) {
      parent_id = bone_map.at(parent_name);
    }

    const auto bone_id = bone_map.at(node_name);
    const auto& inverse_bind_matrix = bone_offsets.at(bone_id);

    const auto local_bind_matrix = _convert_mat4(node->mTransformation);

    skeleton.add_bone(node_name, {parent_id, local_bind_matrix, inverse_bind_matrix});
  }

  for (auto i = 0u; i < node->mNumChildren; ++i) {
    _build_skeleton_hierarchy(node->mChildren[i], node_name, bone_map, bone_offsets, skeleton);
  }
}

static auto _apply_weights(const aiScene* scene, mesh::mesh_data& data, const bone_map& bone_map) -> void {
  for (auto i = 0u; i < scene->mNumMeshes; ++i) {
    const auto* mesh = scene->mMeshes[i];
    const auto& submesh = data.submeshes[i];

    for (auto j = 0u; j < mesh->mNumBones; ++j) {
      const auto* bone = mesh->mBones[j];

      auto itr = bone_map.find(bone->mName.C_Str());

      if (itr == bone_map.end()) {
        continue;
      }

      auto id = itr->second;

      for (auto k = 0u; k < bone->mNumWeights; ++k) {
        const auto& weight = bone->mWeights[k];
        auto& vertex = data.vertices[weight.mVertexId + submesh.vertex_offset];

        for (int l = 0; l < 4; ++l) {
          if (vertex.bone_weights[l] == 0.0f) {
            vertex.bone_ids[l] = id;
            vertex.bone_weights[l]  = weight.mWeight;
            break;
          }
        }
      }
    }
  }
}

auto mesh::_load(const std::filesystem::path& path) -> mesh_data {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error{"Mesh file not found: " + path.string()};
  }

  auto timer = utility::timer{};

  auto data = mesh::mesh_data{};

  static const auto import_flags =
    aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
    aiProcess_Triangulate |             // Make sure we're triangles
    aiProcess_SortByPType |             // Split meshes by primitive type
    aiProcess_GenSmoothNormals |              // Make sure we have legit normals
    aiProcess_GenUVCoords |             // Convert UVs if required
    aiProcess_OptimizeMeshes |          // Batch draws where possible
    aiProcess_JoinIdenticalVertices |
    aiProcess_LimitBoneWeights |        // If more than N (=4) bone weights, discard least influencing bones and renormalise sum to 1
    aiProcess_GlobalScale |             // e.g. convert cm to m for fbx import (and other formats where cm is native)
    aiProcess_ValidateDataStructure |   // Validation 
    aiProcess_OptimizeGraph; 

  auto importer = Assimp::Importer{};

  const auto* scene = importer.ReadFile(path.string(), import_flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    throw std::runtime_error{fmt::format("Error loading mesh '{}': {}", path.string(), importer.GetErrorString())};
  }

  loaded_skeleton = animations::skeleton{};

  auto bone_map = animations::bone_map{};
  auto bone_offsets = animations::bone_offsets{};

  _load_node(scene->mRootNode, scene, data, bone_map, bone_offsets);
  _apply_weights(scene, data, bone_map);

  _build_skeleton_hierarchy(scene->mRootNode, "", bone_map, bone_offsets, loaded_skeleton);

  const auto& root_name = std::string{scene->mRootNode->mName.C_Str()};
  loaded_skeleton.set_inverse_root_transform(math::matrix4x4::inverted(_convert_mat4(scene->mRootNode->mTransformation)));

  const auto vertices_count = data.vertices.size();
  const auto indices_count = data.indices.size();

  const auto b = units::byte{vertices_count * sizeof(vertex3d)};

  const auto kb = units::quantity_cast<units::kilobyte>(b);

  utility::logger<"models">::debug("Loaded mesh: {}, vertices: {}, indices: {}, size: {} kb in {:.2f}ms", path.string(), vertices_count, indices_count, kb.value(), units::quantity_cast<units::millisecond>(timer.elapsed()).value());

  return data;
}

} // namespace sbx::animation
