#include "shader.hpp"

#include <glad/glad.h>

#include <core/logger.hpp>

#include <utils/file_reader.hpp>

namespace sbx {

shader::shader(const std::string& vertex_path, const std::string& fragment_path)
: _program_id(0),
  _uniform_locations{} {
  _load(vertex_path, fragment_path);
}

shader::~shader() {
  glDeleteProgram(_program_id);
}

void shader::bind() const {
  glUseProgram(_program_id);
}

void shader::unbind() const {
  glUseProgram(0);
}

void shader::set_int32(const std::string& name, int32 value) {
  glUniform1i(_get_uniform_location(name), value);
}

void shader::set_float32(const std::string& name, float32 value) {
  glUniform1f(_get_uniform_location(name), value);
}

void shader::set_vector2(const std::string& name, const vector2& value) {
  glUniform2fv(_get_uniform_location(name), 1, value_ptr(value));
}

void shader::set_vector3(const std::string& name, const vector3& value) {
  glUniform3fv(_get_uniform_location(name), 1, value_ptr(value));
}

void shader::set_vector4(const std::string& name, const vector4& value) {
  glUniform4fv(_get_uniform_location(name), 1, value_ptr(value));
}

void shader::set_matrix3x3(const std::string& name, const matrix3x3& value) {
  glUniformMatrix3fv(_get_uniform_location(name), 1, GL_FALSE, value_ptr(value));
}

void shader::set_matrix4x4(const std::string& name, const matrix4x4& value) {
  glUniformMatrix4fv(_get_uniform_location(name), 1, GL_FALSE, value_ptr(value));
}

void shader::_load(const std::string& vertex_path, const std::string& fragment_path) {
  const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  const auto vertex_source = read_file(vertex_path);
  const auto vertex_c_str = vertex_source.c_str();
  const auto fragment_source = read_file(fragment_path);
  const auto fragment_c_str = fragment_source.c_str();

  auto success = 0;

  glShaderSource(vertex_shader, 1, &vertex_c_str, nullptr);
  glCompileShader(vertex_shader);

  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    auto info_log = std::array<char, 512>{};

    // Sometimes c apis are cursed :/
    glGetShaderInfoLog(vertex_shader, info_log.size(), nullptr, info_log.data());

    logger::error("Vertex shader compilation failed: {}", std::string{info_log.data()});

    assert(false); // Vertex shader compilation failed 
  }

  glShaderSource(fragment_shader, 1, &fragment_c_str, nullptr);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    auto info_log = std::array<char, 512>{};

    glGetShaderInfoLog(fragment_shader, info_log.size(), nullptr, info_log.data());

    logger::error("Fragment shader compilation failed: {}", std::string{info_log.data()});

    assert(false); // Vertex shader compilation failed 
  }

  _program_id = glCreateProgram();

  glAttachShader(_program_id, vertex_shader);
  glAttachShader(_program_id, fragment_shader);

  glLinkProgram(_program_id);

  glDetachShader(_program_id, vertex_shader);
  glDetachShader(_program_id, fragment_shader);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

gl_uniform_location shader::_get_uniform_location(const std::string& name) {
  if (_uniform_locations.find(name) == _uniform_locations.cend()) {
    _uniform_locations.emplace(name, glGetUniformLocation(_program_id, name.c_str()));
  }

  return _uniform_locations[name];
}

} // namespace sbx
