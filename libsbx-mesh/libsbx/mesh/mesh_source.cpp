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

  for (auto i = 0; i < scene->mNumMeshes; ++i) {
    const auto* mesh = scene->mMeshes[i];

    
  }
}

} // namespace sbx::mesh
