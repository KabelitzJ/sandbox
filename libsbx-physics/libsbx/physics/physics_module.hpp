#ifndef LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
#define LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <libsbx/units/time.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::pre);

public:

  physics_module() = default;

  ~physics_module() override = default;

  auto update() -> void override {

  }

private:

  

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_