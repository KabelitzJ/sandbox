#ifndef LIBSBX_SCENES_SCENE_MODULE_HPP_
#define LIBSBX_SCENES_SCENE_MODULE_HPP_

#include <memory>

#include <libsbx/core/module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/camera.hpp>

namespace sbx::scenes {

class scenes_module final : public core::module<scenes_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<graphics::graphics_module>{});

public:

  scenes_module() {
    _scene = std::make_unique<scenes::scene>();
  }

  ~scenes_module() override = default;

  auto update() -> void override {

  }

  auto scene() const -> scenes::scene& {
    return *_scene;
  }

private:

  std::unique_ptr<scenes::scene> _scene;

}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
