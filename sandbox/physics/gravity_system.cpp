#include "gravity_system.hpp"

#include <types/transform.hpp>

#include "rigidbody.hpp"
#include "constants.hpp"

namespace sbx {

void gravity_system::initialize() {

}

void gravity_system::update(const time delta_time) {
  // [NOTE] KAJ 2021-11-18 17:16 - https://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet
  // [NOTE] KAJ 2021-11-18 17:16 - https://gamedev.stackexchange.com/questions/15708/how-can-i-implement-gravity/41917#41917
  // [NOTE] KAJ 2021-11-18 17:17 - https://medium.com/@brazmogu/physics-for-game-dev-a-platformer-physics-cheatsheet-f34b09064558

  auto view = create_view<transform, rigidbody>();

  for (const auto entity : view) {
    auto [transform, rigidbody] = view.get(entity);

    if (rigidbody.is_static) {
      continue;
    }

    const auto acceleration = gravity / rigidbody.mass;

    rigidbody.velocity += acceleration * delta_time;

    transform.position += rigidbody.velocity * delta_time;
  }
}

void gravity_system::terminate() {

}

} // namespace sbx
