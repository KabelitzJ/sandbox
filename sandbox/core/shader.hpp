#ifndef SBX_CORE_SHADER_HPP_
#define SBX_CORE_SHADER_HPP_

#include <filesystem>
#include <string>

#include <glad/glad.h>

namespace sbx {

class shader {

public:
  shader(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code);
  ~shader();

  void bind() const;
  void unbind() const;

  GLuint id() const;

private:
  std::string _read_file(const std::filesystem::path& path);
  void _initialize(const std::filesystem::path& vertex_code, const std::filesystem::path& fragment_code);

  GLuint _id;

}; // class shader

} // namespace sbx

#endif // SBX_CORE_SHADER_HPP_
