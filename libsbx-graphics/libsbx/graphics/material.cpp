#include <libsbx/graphics/material.hpp>

namespace sbx::graphics {

material::material(const tinyobj::attrib_t& attributes, const tinyobj::material_t& material) {
  _ambient = math::color{material.ambient[0], material.ambient[1], material.ambient[2], 1.0f};
  _diffuse = math::color{material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0f};
  _specular = math::color{material.specular[0], material.specular[1], material.specular[2], 1.0f};
  _shininess = material.shininess;
}

} // namespace sbx::graphics
