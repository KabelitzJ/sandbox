#include "shader.hpp"

#include "utils/file_reader.hpp"

namespace sbx {

shader::shader(const std::string& vertex_path, const std::string& fragment_path)
: _id(0) {
  _load(vertex_path, fragment_path);
}

shader::~shader() {
  glDeleteProgram(_id);
}

GLuint shader::id() const {
    return _id;
}

void shader::_load(const std::string& vertex_path, const std::string& fragment_path) {
  const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  const auto vertex_source = read_file(vertex_path).c_str();
  const auto fragment_source = read_file(fragment_path).c_str();

  // [NOTE] KAJ 2021-11-05 12:23 - Figure out how we could add logging here? Maybe redesign resource loading? Or static logger?

  glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
  glCompileShader(vertex_shader);

  glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
  glCompileShader(fragment_shader);

  _id = glCreateProgram();

  glAttachShader(_id, vertex_shader);
  glAttachShader(_id, fragment_shader);
  glLinkProgram(_id);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}


} // namespace sbx
