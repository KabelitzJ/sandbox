#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "constants.hpp"

namespace sbx {

camera::camera(const glm::vec3& position, const glm::vec3& direction, float field_of_view, float pitch, float yaw)
: _position(position),
  _direction(direction),
  _field_of_view(field_of_view),
  _pitch(pitch),
  _yaw(yaw) {

}

glm::mat4 camera::view() const {
  return glm::lookAt(_position, _position + _direction, VECTOR_UP);
}

float camera::field_of_view() const {
  return _field_of_view;
}

void camera::on_key_pressed(int key_code) {

}

void camera::on_key_released(int key_code) {

}

void camera::on_mouse_event(mouse_event* event) {
  
}

} // namespace sbx
