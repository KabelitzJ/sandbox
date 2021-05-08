#ifndef SBX_CORE_MESH_LOADER_HPP_
#define SBX_CORE_MESH_LOADER_HPP_

#include <vector>
#include <filesystem>

#include <glad/glad.h>

namespace sbx {

struct basic_mesh {
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
};

basic_mesh load_basic_mesh(const std::filesystem::path& path);

} // namespace sbx

#endif // SBX_CORE_MESH_LOADER_HPP_
