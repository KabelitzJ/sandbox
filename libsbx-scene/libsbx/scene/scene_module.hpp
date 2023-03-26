#ifndef LIBSBX_SCENE_SCENE_MODULE_HPP_
#define LIBSBX_SCENE_SCENE_MODULE_HPP_

#include <libsbx/core/module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::scene {

class scene_module final : public core::module<scene_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<graphics::graphics_module>{});

public:

  scene_module() = default;

  ~scene_module() override = default;

  auto update([[maybe_unused]] std::float_t delta_time) -> void override {

  }

private:



}; // class scene_module

} // namespace sbx::scene

#endif // LIBSBX_SCENE_SCENE_MODULE_HPP_
