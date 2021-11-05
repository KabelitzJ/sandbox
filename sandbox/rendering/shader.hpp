#ifndef SBX_RENDERING_SHADER_HPP_
#define SBX_RENDERING_SHADER_HPP_

#include <string>

#include <glad/glad.h>

namespace sbx {

class shader {

public:
  explicit shader(const std::string& vertex_path, const std::string& fragment_path);
  ~shader();

  GLuint id() const;

private:

  void _load(const std::string& vertex_path, const std::string& fragment_path);

  GLuint _id{};

};

} // namespace sbx

#endif // SBX_RENDERING_SHADER_HPP_
