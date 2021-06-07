#ifndef SBX_CORE_TEXTURE_HPP_
#define SBX_CORE_TEXTURE_HPP_

#include <glad/glad.h>

#include "base_resource.hpp"

namespace sbx {

class texture : public base_resource {

public:
  texture(const std::filesystem::path& path);
  ~texture();

  void bind() const;
  void unbind() const;

  GLuint id() const;
  unsigned int unit() const;

private:
  texture();

  base_resource_data* _load(const std::filesystem::path& path) override;
  void _initialize(base_resource_data* resource_data) override;

  static unsigned int _texture_unit_counter;

  GLuint _id;
  unsigned int _texture_unit;

}; // class texture

} // namespace sbx

#endif // SBX_CORE_TEXTURE_HPP_
