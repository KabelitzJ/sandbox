#ifndef SBX_CORE_OBJECT_HPP_
#define SBX_CORE_OBJECT_HPP_

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "shader.hpp"

namespace sbx {

class object {

public:
  object(const mesh& mesh, const texture& texture, const transform& transform) : _mesh(mesh), _texture(texture), _transform(transform) {}
  ~object() = default;

  glm::mat4 model() const {
    glm::mat4 model(1.0f);

    model = glm::translate(model, _transform.position);
    model = glm::rotate(model, glm::radians(_transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, _transform.scale);

    return model;
  }

  void draw(shader& shader) {
    _texture.bind();
    shader.set_uniform_1i("uni_texture", _texture.unit());

    _mesh.draw(/*shader*/);
  }

private:
  mesh _mesh;
  texture _texture;
  transform _transform;

}; // class object

} // namespace sbx

#endif // SBX_CORE_OBJECT_HPP_
