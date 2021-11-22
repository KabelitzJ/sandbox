#ifndef SBX_RENDERING_TEXTURE_HPP_
#define SBX_RENDERING_TEXTURE_HPP_

#include <string>

#include <types/gl.hpp>

namespace sbx {

class texture {

public:

  texture(const std::string& path);
  ~texture();

  void bind(const gl_texture_unit texture_unit) const;

private:

  void _load(const std::string& path);

  gl_texture _texture_id;

}; // class texture

} // namespace sbx

#endif // SBX_RENDERING_TEXTURE_HPP_
