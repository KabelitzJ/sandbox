#ifndef LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
#define LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <cmath>
#include <optional>

#include <libsbx/units/time.hpp>

#include <libsbx/math/transform.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>
#include <libsbx/core/logger.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/tag.hpp>

#include <libsbx/models/mesh.hpp>

#include <libsbx/physics/rigidbody.hpp>
#include <libsbx/physics/box_collider.hpp>

namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::pre, dependencies<scenes::scenes_module>{});

public:

  physics_module() {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    scenes_module.register_component_loader("rigidbody", [](scenes::node& node, const YAML::Node& node_data){
      const auto mass = node_data["mass"].as<std::float_t>();
      const auto bounce = node_data["bounce"].as<std::float_t>();
      const auto is_static = node_data["is_static"].as<bool>();

      node.add_component<physics::rigidbody>(mass, bounce, is_static);
    });

    scenes_module.register_component_loader("box_collider", [](scenes::node& node, const YAML::Node& node_data){
      const auto size = node_data["size"].as<math::vector3>();

      node.add_component<physics::box_collider>(size);
    });

    // [NOTE] KAJ 2023-11-23 : This should not be done here but libsbx::models does not have a module
    scenes_module.register_asset_loader("mesh", [](const YAML::Node& node_data) {
      auto& assets_module = core::engine::get_module<assets::assets_module>();

      const auto path = node_data.as<std::string>();

      assets_module.load_asset<models::mesh>(std::filesystem::path{path});
    });
  }

  ~physics_module() override = default;

  auto update() -> void override {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::delta_time();

    auto rigidbody_nodes = scene.query<rigidbody>();
    auto collider_nodes = scene.query<box_collider>();

    for (auto& node : rigidbody_nodes) {
      auto& rigidbody = node.get_component<physics::rigidbody>();

      if (rigidbody.is_static()) {
        continue;
      }

      auto& transform = node.get_component<math::transform>();

      if (node.has_component<physics::box_collider>()) {
        for (const auto& collider_node : collider_nodes) {
          if (node == collider_node) {
            continue;
          }

          if (auto info = _test_collision(node, collider_node); info) {
            auto& other_rigidbody = collider_node.get_component<physics::rigidbody>();

            const auto bounce = rigidbody.bounce() + other_rigidbody.bounce();
            const auto velocity = rigidbody.velocity() * bounce;

            if (velocity.length_squared() > 0.01f) {
              rigidbody.set_velocity(-velocity);
            } else {
              rigidbody.set_velocity(math::vector3::zero);
            }
          }
        }
      } 

      const auto position = transform.position() + rigidbody.velocity() * delta_time.value();
      transform.set_position(position);

      const auto velocity = rigidbody.velocity() + rigidbody.acceleration() * delta_time.value();
      rigidbody.set_velocity(velocity);
    }
  }

private:

  struct collision_info {

  }; // struct collision_info

  auto _test_collision(const scenes::node& a, const scenes::node& b) -> std::optional<collision_info> {
    const auto& a_transform = a.get_component<math::transform>();
    const auto& b_transform = b.get_component<math::transform>();

    const auto& a_collider = a.get_component<physics::box_collider>();
    const auto& b_collider = b.get_component<physics::box_collider>();

    const auto a_min = a_transform.position() - a_collider.size() / 2.0f;
    const auto a_max = a_transform.position() + a_collider.size() / 2.0f;

    const auto b_min = b_transform.position() - b_collider.size() / 2.0f;
    const auto b_max = b_transform.position() + b_collider.size() / 2.0f;

    if (a_min.x > b_max.x || a_max.x < b_min.x) {
      return std::nullopt;
    }

    if (a_min.y > b_max.y || a_max.y < b_min.y) {
      return std::nullopt;
    }

    if (a_min.z > b_max.z || a_max.z < b_min.z) {
      return std::nullopt;
    }

    return collision_info{};
  }

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
