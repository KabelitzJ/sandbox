#include "shader.hpp"

#include <sstream>
#include <fstream>

namespace sbx {

shader::shader(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code) : _id(0) {
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

} // namespace sbx
