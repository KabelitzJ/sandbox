#ifndef SBX_CORE_CAMERA_HPP_
#define SBX_CORE_CAMERA_HPP_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace sbx {

class camera {

public:
  camera(const glm::vec3& position, const glm::vec3& direction, float field_of_view, float pitch = 0.0f, float yaw = 0.0f);
  virtual ~camera() = default;

  virtual glm::mat4 projection() const = 0;

  glm::mat4 view() const;

protected:
  float field_of_view() const;

private:
  static constexpr glm::vec3 _UP = glm::vec3(0.0f, 1.0f, 0.0f);

  glm::vec3 _position;
  glm::vec3 _direction;
  float _field_of_view;
  float _pitch;
  float _yaw;

}; // class camera

} // namespace sbx

#endif // SBX_CORE_CAMERA_HPP_