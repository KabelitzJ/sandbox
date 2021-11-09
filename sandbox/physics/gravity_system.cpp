#include "gravity_system.hpp"

#include <core/transform.hpp>

#include "rigidbody.hpp"
#include "constants.hpp"

namespace sbx {

gravity_system::gravity_system(scene* scene)
: _scene{scene} { }

void gravity_system::initialize() {

}

void gravity_system::update(const time delta_time) {
  auto view = _scene->create_view<transform, rigidbody>();

  for (const auto entity : view) {
    auto [transform, rigidbody] = view.get(entity);

    const auto acceleration = gravity / rigidbody.mass;

    rigidbody.velocity = acceleration * delta_time;

    transform.position = rigidbody.velocity * delta_time;
  }
}

void gravity_system::terminate() {

}

} // namespace sbx
