#ifndef LIBSBX_SCENES_HIERARCHY_MODULE_HPP_
#define LIBSBX_SCENES_HIERARCHY_MODULE_HPP_

#include <libsbx/core/module.hpp>

namespace sbx::scenes {

class heirarchy_module final : public core::module<heirarchy_module> {

  friend class scene;

  inline static const auto is_registered = register_module(stage::post);

public:

  heirarchy_module() {
    
  }

  ~heirarchy_module() override = default;

  auto update() -> void override {

  }

}; // class heirarchy_module

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_HIERARCHY_MODULE_HPP_
