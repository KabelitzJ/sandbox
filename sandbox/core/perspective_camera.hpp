#ifndef SBX_CORE_PERSPECTIE_CAMERA_HPP_
#define SBX_CORE_PERSPECTIE_CAMERA_HPP_

#include <evtsys/window_event.hpp>

#include "camera.hpp"

namespace sbx {

class perspective_camera : public camera {

public:
  perspective_camera(const glm::vec3& position, const glm::vec3& direction, float speed, float field_of_view, float aspect_ratio, float near, float far, float pitch = 0.0f, float yaw = 0.0f);
  ~perspective_camera() = default;

  glm::mat4 projection() const override;

private:
  void on_framebuffer_resize(framebuffer_resized_event& event);

  float _aspect_ratio;
  float _near;
  float _far;

}; // class perspective_camera

} // namespace sbx

#endif // SBX_CORE_PERSPECTIE_CAMERA_HPP_
