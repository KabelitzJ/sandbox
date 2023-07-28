#ifndef LIBSBX_MODELS_MATERIAL_HPP_
#define LIBSBX_MODELS_MATERIAL_HPP_

#include <libsbx/math/color.hpp>

#include <tiny_obj_loader.h>

namespace sbx::models {

class material {

public:

  material(const tinyobj::attrib_t& attributes, const tinyobj::material_t& material);

  ~material() = default;

  auto ambient() const noexcept -> const math::color& {
    return _ambient;
  }

  auto diffuse() const noexcept -> const math::color& {
    return _diffuse;
  }

  auto specular() const noexcept -> const math::color& {
    return _specular;
  }

  auto shininess() const noexcept -> std::float_t {
    return _shininess;
  }

private:

  math::color _ambient;
  math::color _diffuse;
  math::color _specular;
  std::float_t _shininess;

}; // class material

} // namespace sbx::models

#endif // LIBSBX_MODELS_MATERIAL_HPP_
