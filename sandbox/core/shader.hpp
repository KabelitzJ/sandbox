#ifndef SBX_CORE_SHADER_HPP_
#define SBX_CORE_SHADER_HPP_

#include <filesystem>

#include <glad/glad.h>

namespace sbx {

class shader {

public:
  shader(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code);
  ~shader();

  void bind() const;
  void unbind() const;

private:
  void _initialize(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code);
  void _terminate();

  GLuint _id;

}; // class shader

} // namespace sbx

#endif // SBX_CORE_SHADER_HPP_