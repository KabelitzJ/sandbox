#ifndef LIBSBX_SCENES_SCENE_MODULE_HPP_
#define LIBSBX_SCENES_SCENE_MODULE_HPP_

#include <libsbx/core/module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::scenes {

class scenes_module final : public core::module<scenes_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<graphics::graphics_module>{});

public:

  scenes_module() = default;

  ~scenes_module() override = default;

  auto update([[maybe_unused]] std::float_t delta_time) -> void override {

  }

private:



}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
