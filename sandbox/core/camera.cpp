#include "camera.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "constants.hpp"

namespace sbx {

camera::camera(const glm::vec3& position, const glm::vec3& direction, float speed, float field_of_view, float pitch, float yaw)
: _is_first_cursor_movement(true),
  _sensitivity(0.4f),
  _last_cursor_position(0.0f, 0.0f),
  _position(position),
  _direction(direction),
  _speed(speed),
  _field_of_view(field_of_view),
  _pitch(pitch),
  _yaw(yaw) {

}

glm::mat4 camera::view() const {
  auto foo = glm::lookAt(_position, _position + _direction, VECTOR_UP);
  return foo;
}

void camera::update(const input_manager& input) {
  _update_position(input);
  _update_direction(input);
}

float camera::field_of_view() const {
  return _field_of_view;
}

void camera::_update_position(const input_manager& input) {
  if (input.is_key_pressed(key_code::W)) {
    _position += _direction * _speed;
  }
  if (input.is_key_pressed(key_code::S)) {
    _position -= _direction * _speed;
  }
  if (input.is_key_pressed(key_code::A)) {
    _position -= glm::normalize(glm::cross(_direction, VECTOR_UP)) * _speed;
  }
  if (input.is_key_pressed(key_code::D)) {
    _position += glm::normalize(glm::cross(_direction, VECTOR_UP)) * _speed;
  }
  if (input.is_key_pressed(key_code::Q)) {
    _position -= VECTOR_UP * _speed;
  }
  if (input.is_key_pressed(key_code::E)) {
    _position += VECTOR_UP * _speed;
  }
}

void camera::_update_direction(const input_manager& input) {
  // center the view to the origin
  if (input.is_key_pressed(key_code::SPACE)) {
    _direction = glm::vec3(0.0f, 0.0f, 0.0f) - _position;
    return;
  }

  const glm::vec2& mouse_position = input.mouse_position();

  // check here if mouse has been moved at all
  if (_is_first_cursor_movement && (mouse_position.x != 0.0f || mouse_position.y != 0.0f)) {
    _last_cursor_position = mouse_position;
    _is_first_cursor_movement = false;
  }

  glm::vec2 cursor_offset(mouse_position.x - _last_cursor_position.x, _last_cursor_position.y - mouse_position.y);
  cursor_offset *= _sensitivity;

  _last_cursor_position = glm::vec2(mouse_position.x, mouse_position.y);

  _yaw += cursor_offset.x;
  _pitch += cursor_offset.y;

  if(_pitch > 89.0f) {
    _pitch =  89.0f;
  }
  if(_pitch < -89.0f) {
    _pitch = -89.0f;
  }

  glm::vec3 direction;
  direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  direction.y = sin(glm::radians(_pitch));
  direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  _direction = glm::normalize(direction);
}


} // namespace sbx
