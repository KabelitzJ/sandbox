#ifndef SBX_CORE_TEXTURE_HPP_
#define SBX_CORE_TEXTURE_HPP_

#include <glad/glad.h>

#include "base_resource.hpp"

namespace sbx {

class texture : public base_resource {

public:
  texture(const std::filesystem::path& path);
  ~texture();

  void bind(unsigned int unit);
  void unbind();

  GLuint id() const;

private:
  texture();

  base_resource_data* _load(const std::filesystem::path& path) override;
  void _initialize(base_resource_data* resource_data) override;

  GLuint _id;
  unsigned int _active_unit;

}; // class texture

} // namespace sbx

#endif // SBX_CORE_TEXTURE_HPP_
