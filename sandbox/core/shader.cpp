#include "shader.hpp"

#include <sstream>

#include "file_utils.hpp"

namespace sbx {

shader::shader(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code) : _id(0) {
  _initialize(vertex_code, fragment_code);
}

shader::~shader() {
  _terminate();
}

void shader::bind() const {
  glUseProgram(_id);
}

void shader::unbind() const {
  glUseProgram(0);
}

void shader::_initialize(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code) {
  std::string vertex_source = read_file_contents(vertex_code);
  std::string fragment_source = read_file_contents(fragment_code);

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

    std::stringstream ss;

    ss << "Could not compile vertex shader from file: '" << vertex_code << "'!\n" 
       << "Log: " << info_log << "\n";

    throw std::runtime_error(ss.str());
  }

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source_cstr, nullptr);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);

    std::stringstream ss;

    ss << "Could not compile fragment shader from file: '" << fragment_code << "'!\n" 
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

    std::stringstream ss;

    ss << "Could not link shader program!\n"
       << "Log: " << info_log << "\n";
    
    throw std::runtime_error(ss.str());
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void shader::_terminate() {
  glUseProgram(0);
  glDeleteProgram(_id);
}

} // namespace sbx
