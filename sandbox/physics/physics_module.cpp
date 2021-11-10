#include "physics_module.hpp"

#include "gravity_system.hpp"

namespace sbx {

physics_module::physics_module() { }

void physics_module::initialize() {
  _scheduler->add_system<gravity_system>();
}

void physics_module::terminate() {

}

} // namespace sbx