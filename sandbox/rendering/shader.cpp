#include "shader.hpp"

#include <glad/glad.h>

#include <core/logger.hpp>

#include <utils/file_reader.hpp>

namespace sbx {

shader::shader(const std::string& vertex_path, const std::string& fragment_path)
: _id(0),
  _uniform_locations{} {
  _load(vertex_path, fragment_path);
}

shader::~shader() {
  glDeleteProgram(_id);
}

void shader::bind() const {
  glUseProgram(_id);
}

void shader::unbind() const {
  glUseProgram(0);
}

void shader::set_int32(const std::string& name, int value) {
  glUniform1i(_get_uniform_location(name), value);
}

void shader::set_float32(const std::string& name, float value) {
  glUniform1f(_get_uniform_location(name), value);
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
    auto info_log = std::string{};
    info_log.reserve(512);

    // Sometimes c apis are cursed :/
    glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log.data());

    logger::error("Vertex shader compilation failed: {}", info_log);

    assert(false); // Vertex shader compilation failed 
  }

  glShaderSource(fragment_shader, 1, &fragment_c_str, nullptr);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    auto info_log = std::string{};
    info_log.reserve(512);

    glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log.data());

    logger::error("Fragment shader compilation failed: {}", info_log);

    assert(false); // Vertex shader compilation failed 
  }

  _id = glCreateProgram();

  glAttachShader(_id, vertex_shader);
  glAttachShader(_id, fragment_shader);

  glLinkProgram(_id);

  glDetachShader(_id, vertex_shader);
  glDetachShader(_id, fragment_shader);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

int32 shader::_get_uniform_location(const std::string& name) {
  if (_uniform_locations.find(name) == _uniform_locations.cend()) {
    _uniform_locations.emplace(name, glGetUniformLocation(_id, name.c_str()));
  }

  return _uniform_locations.at(name);
}

} // namespace sbx
