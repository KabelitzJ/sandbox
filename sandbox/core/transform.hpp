#ifndef SBX_CORE_TRANSFORM_HPP_
#define SBX_CORE_TRANSFORM_HPP_

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sbx {

struct transform {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
}; // struct transform

class custom_transform {

public:
  custom_transform() {

  }

  ~custom_transform() = default;

  glm::mat4 transformation() const {
    return _transformation;
  }

  void update() {
    _transformation = glm::identity<glm::mat4>();

    _transformation = glm::translate(_transformation, _position);
    _transformation = _transformation * glm::mat4_cast(_rotation);
    _transformation = glm::scale(_transformation, _scale);
  }

  void translate(const glm::vec3& position) {
    _position += position;
  }

  void rotate(const glm::vec3& axis, float angle) {
    _rotation += glm::angleAxis(angle, axis);
  }

  void scale(const glm::vec3& scale) {
    _scale += scale;
  }

private:
  glm::vec3 _position;
  glm::quat _rotation;
  glm::vec3 _scale;
  glm::mat4 _transformation;

};

} // namespace sbx

#endif // SBX_CORE_TRANSFORM_HPP_
