#ifndef LIBSBX_SCENES_SCENE_HPP_
#define LIBSBX_SCENES_SCENE_HPP_

#include <memory>

#include <libsbx/scenes/camera.hpp>

namespace sbx::scenes {

class scene {

public:

  scene();

  ~scene();

  auto camera() const -> const scenes::camera& {
    return *_camera;
  }

private:

  std::unique_ptr<scenes::camera> _camera;

}; // class scene

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_HPP_
