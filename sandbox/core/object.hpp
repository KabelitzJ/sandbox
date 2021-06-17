#ifndef SBX_CORE_OBJECT_HPP_
#define SBX_CORE_OBJECT_HPP_

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "mesh.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "shader.hpp"

namespace sbx {

struct material {
  texture* diffuse;
  texture* specular;
}; // struct material

class object {

public:
  object(const mesh& mesh, const material& material, const transform& transform) : _mesh(mesh), _material(material), _model(1.0f) {
    _initialize_model(transform);
  }
  ~object() = default;

  glm::mat4 model() const {
    return _model;
  }

  void rotate(const glm::vec3& rotation, float angle) {
    _model = glm::rotate(_model, glm::radians(angle), rotation);
  }

  void draw(shader& shader) {
    _material.diffuse->bind(0);
    shader.set_uniform_1i("uni_material.diffuse", 0);

    _material.specular->bind(1);
    shader.set_uniform_1i("uni_material.specular", 0);


    _mesh.draw(/*shader*/);

    _material.diffuse->unbind();
    _material.specular->unbind();
  }

private:
  void _initialize_model(const transform& transform) {
    _model = glm::translate(_model, transform.position);
    _model = glm::rotate(_model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    _model = glm::rotate(_model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    _model = glm::rotate(_model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    _model = glm::scale(_model, transform.scale);
  }

  mesh _mesh;
  material _material;
  glm::mat4 _model;

}; // class object

} // namespace sbx

#endif // SBX_CORE_OBJECT_HPP_
