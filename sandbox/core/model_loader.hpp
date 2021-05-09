#ifndef SBX_CORE_MESH_LOADER_HPP_
#define SBX_CORE_MESH_LOADER_HPP_

#include <vector>
#include <filesystem>

#include <glad/glad.h>

namespace sbx {

struct basic_model {
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
};

basic_model load_basic_model(const std::filesystem::path& path);

} // namespace sbx

#endif // SBX_CORE_MESH_LOADER_HPP_
