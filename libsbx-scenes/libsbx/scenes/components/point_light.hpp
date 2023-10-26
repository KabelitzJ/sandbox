#ifndef LIBSBX_SCENES_COMPONENTS_POINT_LIGHT_HPP_
#define LIBSBX_SCENES_COMPONENTS_POINT_LIGHT_HPP_

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/color.hpp>

namespace sbx::scenes {

class point_light {

public:

  point_light(math::color ambient, math::color diffuse, math::color specular, std::float_t constant, std::float_t linear, std::float_t quadratic)
  : _ambient{ambient}, 
    _diffuse{diffuse}, 
    _specular{specular}, 
    _constant{constant}, 
    _linear{linear}, 
    _quadratic{quadratic} { }

  auto ambient() const noexcept -> const math::color& {
    return _ambient;
  }

  auto set_ambient(const math::color& ambient) noexcept -> void {
    _ambient = ambient;
  }
  
  auto diffuse() const noexcept -> const math::color& {
    return _diffuse;
  }

  auto set_diffuse(const math::color& diffuse) noexcept -> void {
    _diffuse = diffuse;
  }

  auto specular() const noexcept -> const math::color& {
    return _specular;
  }

  auto set_specular(const math::color& specular) noexcept -> void {
    _specular = specular;
  }

  auto constant() const noexcept -> std::float_t {
    return _constant;
  }

  auto set_constant(std::float_t constant) noexcept -> void {
    _constant = constant;
  }

  auto linear() const noexcept -> std::float_t {
    return _linear;
  }

  auto set_linear(std::float_t linear) noexcept -> void {
    _linear = linear;
  }

  auto quadratic() const noexcept -> std::float_t {
    return _quadratic;
  }

  auto set_quadratic(std::float_t quadratic) noexcept -> void {
    _quadratic = quadratic;
  }

private:

  math::color _ambient;
  math::color _diffuse; 
  math::color _specular;

  std::float_t _constant;
  std::float_t _linear;
  std::float_t _quadratic;

}; // class point_light

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_POINT_LIGHT_HPP_
