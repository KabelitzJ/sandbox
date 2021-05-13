#ifndef SBX_CORE_TEXTURE_HPP_
#define SBX_CORE_TEXTURE_HPP_

#include <filesystem>

#include <glad/glad.h>

namespace sbx {

class texture {

public:
  texture(const std::filesystem::path& path);
  ~texture();

  void bind() const;
  void unbind() const;

  GLuint id() const;

private:
  void _initialize(const std::filesystem::path& path);

  static unsigned int _texture_count;

  GLuint _id;

}; // class texture

} // namespace sbx

#endif // SBX_CORE_TEXTURE_HPP_
