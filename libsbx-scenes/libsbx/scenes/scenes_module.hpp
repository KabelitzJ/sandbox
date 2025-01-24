#ifndef LIBSBX_SCENES_SCENE_MODULE_HPP_
#define LIBSBX_SCENES_SCENE_MODULE_HPP_

#include <memory>

#include <yaml-cpp/yaml.h>

#include <libsbx/math/color.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/images/image2d.hpp>

#include <libsbx/scenes/scene.hpp>

#include <libsbx/scenes/components/static_mesh.hpp>
#include <libsbx/scenes/components/point_light.hpp>
#include <libsbx/scenes/components/directional_light.hpp>
#include <libsbx/scenes/components/camera.hpp>

namespace sbx::scenes {

class scenes_module final : public core::module<scenes_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::normal);

public:

  scenes_module()
  : _scene{nullptr} {
    
  }

  ~scenes_module() override = default;

  auto update() -> void override {

  }

  template<typename Type = scenes::scene, typename... Args>
  requires ((std::is_base_of_v<scenes::scene, Type> || std::is_same_v<Type, scenes::scene>) && !std::is_abstract_v<Type> && std::is_constructible_v<Type, Args...>)
  auto create_scene(Args&&... args) -> Type& {
    _scene = std::make_unique<Type>(std::forward<Args>(args)...);

    return *static_cast<Type*>(_scene.get());
  }

  template<typename Type = scenes::scene>
  auto scene() -> Type& {
    return *static_cast<Type*>(_scene.get());
  }

private:

  std::unique_ptr<scenes::scene> _scene;

}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
