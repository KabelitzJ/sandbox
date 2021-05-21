#ifndef SBX_CORE_CAMERA_HPP_
#define SBX_CORE_CAMERA_HPP_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <evtsys/key_event.hpp>
#include <evtsys/mouse_event.hpp>

#include "input_manager.hpp"

namespace sbx {

class camera  {

public:
  camera(const glm::vec3& position, const glm::vec3& direction, float speed, float field_of_view, float pitch = 0.0f, float yaw = 0.0f);
  virtual ~camera() = default;

  virtual glm::mat4 projection() const = 0;

  glm::mat4 view() const;

  void update(const input_manager& input);

protected:
  float field_of_view() const;

private:
  void _update_position(const input_manager& input);
  void _update_direction(const input_manager& input);

  bool _is_first_cursor_movement;
  float _sensitivity;
  glm::vec2 _last_cursor_position;
  glm::vec3 _position;
  glm::vec3 _direction;
  float _speed;
  float _field_of_view;
  float _pitch;
  float _yaw;

  friend class event_queue;
  friend class event_listener;

}; // class camera

} // namespace sbx

#endif // SBX_CORE_CAMERA_HPP_
