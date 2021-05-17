#ifndef SBX_CORE_CAMERA_HPP_
#define SBX_CORE_CAMERA_HPP_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <evtsys/key_event_listener.hpp>
#include <evtsys/mouse_event_listener.hpp>

namespace sbx {

class camera : public key_event_listener, public mouse_event_listener {

public:
  camera(const glm::vec3& position, const glm::vec3& direction, float field_of_view, float pitch = 0.0f, float yaw = 0.0f);
  virtual ~camera() = default;

  virtual glm::mat4 projection() const = 0;

  glm::mat4 view() const;

protected:
  float field_of_view() const;

private:
  void on_key_event(key_event* event) override;
  void on_mouse_event(mouse_event* event) override;

  glm::vec3 _position;
  glm::vec3 _direction;
  float _field_of_view;
  float _pitch;
  float _yaw;

}; // class camera

} // namespace sbx

#endif // SBX_CORE_CAMERA_HPP_
