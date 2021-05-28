#ifndef SBX_CORE_SHADER_HPP_
#define SBX_CORE_SHADER_HPP_

#include <filesystem>
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace sbx {

class shader {

public:
  shader(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code);
  ~shader();

  void bind() const;
  void unbind() const;

  GLuint id() const;


  void set_uniform_1i(const std::string& name, GLint value);
  void set_uniform_1f(const std::string& name, GLfloat value);
  void set_uniform_3f(const std::string& name, const glm::vec3& value);
  void set_uniform_4f(const std::string& name, const glm::vec4& value);
  void set_uniform_matrix_3fv(const std::string& name, const glm::mat3& value);
  void set_uniform_matrix_4fv(const std::string& name, const glm::mat4& value);

private:
  std::string _read_file(const std::filesystem::path& path);
  void _initialize(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code);
  GLint _get_uniform_location(const std::string& uniform_name);

  GLuint _id;
  std::unordered_map<std::string, GLint> _uniform_map;

}; // class shader

} // namespace sbx

#endif // SBX_CORE_SHADER_HPP_
