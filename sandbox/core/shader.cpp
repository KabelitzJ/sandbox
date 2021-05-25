#include "shader.hpp"

#include <sstream>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

namespace sbx {

shader::shader(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code) : _id(0), _uniform_map() {
  _initialize(vertex_code, fragment_code);
}

shader::~shader() {
  glUseProgram(0);
  glDeleteProgram(_id);
}

void shader::bind() const {
  glUseProgram(_id);
}

void shader::unbind() const {
  glUseProgram(0);
}

GLuint shader::id() const {
  return _id;
}

void shader::set_uniform_1i(const std::string& name, GLint value) {
  GLuint location = _get_uniform_location(name);
  glUniform1i(location, value);
}

void shader::set_uniform_3f(const std::string& name, const glm::vec3& value) {
  GLuint location = _get_uniform_location(name);
  glUniform3f(location, value.x, value.y, value.z);
}

void shader::set_uniform_4f(const std::string& name, const glm::vec4& value) {
  GLuint location = _get_uniform_location(name);
  glUniform4f(location, value.x, value.y, value.z, value.w);
}

void shader::set_uniform_matrix_3fv(const std::string& name, const glm::mat3& value) {
  GLuint location = _get_uniform_location(name);
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void shader::set_uniform_matrix_4fv(const std::string& name, const glm::mat4& value) {
  GLuint location = _get_uniform_location(name);
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

std::string shader::_read_file(const std::filesystem::path& path) {
  std::ifstream file_stream(path);

  if (!file_stream.is_open()) {
    std::ostringstream ss;

    ss << "[Error] Could not read shader from file: '" << path << "'!\n";

    throw std::runtime_error(ss.str());
  }

  std::string line;
  std::ostringstream file_conten;

  while (std::getline(file_stream, line)) {
    file_conten << line << '\n';
  }

  return file_conten.str();
}

void shader::_initialize(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code) {
  std::string vertex_source = _read_file(vertex_code);
  std::string fragment_source = _read_file(fragment_code);

  const char* vertex_source_cstr = vertex_source.c_str();
  const char* fragment_source_cstr = fragment_source.c_str();

  int success = 0;
  char info_log[512];

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source_cstr, nullptr);
  glCompileShader(vertex_shader);

  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);

    std::ostringstream ss;

    ss << "[Error] Could not compile vertex shader from file: '" << vertex_code << "'!\n" 
       << "Log: " << info_log << "\n";

    throw std::runtime_error(ss.str());
  }

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source_cstr, nullptr);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);

    std::ostringstream ss;

    ss << "[Error] Could not compile fragment shader from file: '" << fragment_code << "'!\n" 
       << "Log: " << info_log << "\n";

    throw std::runtime_error(ss.str());
  }

  _id = glCreateProgram();
  glAttachShader(_id, vertex_shader);
  glAttachShader(_id, fragment_shader);
  glLinkProgram(_id);

  glGetProgramiv(_id, GL_LINK_STATUS, &success);

  if(!success) {
    glGetProgramInfoLog(_id, 512, NULL, info_log);

    std::ostringstream ss;

    ss << "[Error] Could not link shader program!\n"
       << "Log: " << info_log << "\n";
    
    throw std::runtime_error(ss.str());
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

GLint shader::_get_uniform_location(const std::string& uniform_name) {
  auto uniform = _uniform_map.find(uniform_name);

  if (uniform != _uniform_map.end()) {
    return uniform->second;
  }

  GLint location = glGetUniformLocation(_id, uniform_name.c_str());

  _uniform_map.emplace(uniform_name, location);

  return location;
}

} // namespace sbx
