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

float camera::field_of_view() const {
  return _field_of_view;
}

void camera::register_event_callbacks(event_queue& queue) {
  queue.subscribe<key_pressed_event>(std::bind(&camera::on_key_pressed, this, std::placeholders::_1));
  queue.subscribe<key_repeated_event>(std::bind(&camera::on_key_repeated, this, std::placeholders::_1));
  queue.subscribe<mouse_moved_event>(std::bind(&camera::on_mouse_moved, this, std::placeholders::_1));
}

void camera::on_key_pressed(key_pressed_event& event) {
  const key_code code = event.code();

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

void camera::on_key_repeated(key_repeated_event& event) {
  key_pressed_event temp_event(event.code());
  on_key_pressed(temp_event);
}

void camera::on_mouse_moved(mouse_moved_event& event) {
  const float xpos = event.x();
  const float ypos = event.y();

  if (_is_first_cursor_movement) {
    _last_cursor_position = glm::vec2(xpos, ypos);
    _is_first_cursor_movement = false;
  }

  glm::vec2 cursor_offset(xpos - _last_cursor_position.x, _last_cursor_position.y - ypos);
  cursor_offset *= _sensitivity;

  _last_cursor_position = glm::vec2(xpos, ypos);

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
