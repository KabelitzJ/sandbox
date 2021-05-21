#ifndef SBX_CORE_CAMERA_HPP_
#define SBX_CORE_CAMERA_HPP_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <evtsys/event_listener.hpp>
#include <evtsys/key_event.hpp>
#include <evtsys/mouse_event.hpp>

namespace sbx {

class camera : public event_listener {

public:
  camera(const glm::vec3& position, const glm::vec3& direction, float speed, float field_of_view, float pitch = 0.0f, float yaw = 0.0f);
  virtual ~camera() = default;

  virtual glm::mat4 projection() const = 0;

  glm::mat4 view() const;

protected:
  void register_event_callbacks(event_queue& queue) override;

  float field_of_view() const;

private:
  void on_key_pressed(key_pressed_event& event);
  void on_key_repeated(key_repeated_event& event);
  void on_mouse_moved(mouse_moved_event& event);

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
