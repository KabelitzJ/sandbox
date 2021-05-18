#include "camera.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "constants.hpp"

namespace sbx {

camera::camera(const glm::vec3& position, const glm::vec3& direction, float speed, float field_of_view, float pitch, float yaw)
: _position(position),
  _direction(direction),
  _speed(speed),
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

void camera::on_key_pressed(key_code code) {
  std::cout << static_cast<std::underlying_type_t<key_code>>(code) << '\n';

  if (code == key_code::W) {
    _position += _direction * _speed;
  }
  if (code == key_code::S) {
    _position -= _direction * _speed;
  }
  if (code == key_code::A) {
    _position -= glm::normalize(glm::cross(_direction, VECTOR_UP)) * _speed;
  }
  if (code == key_code::D) {
    _position += glm::normalize(glm::cross(_direction, VECTOR_UP)) * _speed;
  }
  if (code == key_code::Q) {
    _position -= VECTOR_UP * _speed;
  }
  if (code == key_code::E) {
    _position += VECTOR_UP * _speed;
  }
}

void camera::on_key_released(key_code code) {

}

void camera::on_mouse_event(mouse_event* event) {
  
}

} // namespace sbx
