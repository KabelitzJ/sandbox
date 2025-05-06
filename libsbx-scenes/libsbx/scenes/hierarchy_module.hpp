#ifndef LIBSBX_SCENES_HIERARCHY_MODULE_HPP_
#define LIBSBX_SCENES_HIERARCHY_MODULE_HPP_

#include <libsbx/core/module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

namespace sbx::scenes {

class heirarchy_module final : public core::module<heirarchy_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::post);

public:

  heirarchy_module() {
    
  }

  ~heirarchy_module() override = default;

  auto update() -> void override {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();
  }

}; // class heirarchy_module

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_HIERARCHY_MODULE_HPP_
