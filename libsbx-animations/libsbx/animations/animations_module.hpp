#ifndef LIBSBX_ANIMATIONS_ANIMATIONS_MODULE_HPP_
#define LIBSBX_ANIMATIONS_ANIMATIONS_MODULE_HPP_

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/animations/animator.hpp>

namespace sbx::animations {
  
class animations_module : public core::module<animations_module> {

  inline static const auto is_registered = register_module(stage::post);

public:

  animations_module() { }

  ~animations_module() override {

  }

  auto update() -> void override {
    SBX_SCOPED_TIMER("animations_module::update");

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::delta_time();

    auto query = scene.query<animator>();

    for (auto&& [node, animator] : query.each()) {
      animator.update(delta_time);
    }
  }

}; // class assets_module

} // namespace sbx::animations

#endif // LIBSBX_ANIMATIONS_ANIMATIONS_MODULE_HPP_