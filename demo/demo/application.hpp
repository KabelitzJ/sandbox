#ifndef DEMO_APPLICATION_HPP_
#define DEMO_APPLICATION_HPP_

#include <libsbx/units/units.hpp>
#include <libsbx/utility/utility.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/memory/memory.hpp>
#include <libsbx/signals/signals.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>
#include <libsbx/models/models.hpp>
#include <libsbx/scenes/scenes.hpp>
#include <libsbx/ui/ui.hpp>
#include <libsbx/physics/physics.hpp>

#include <demo/camera_controller.hpp>

#include <demo/terrain/mesh.hpp>

namespace demo {

class application : public sbx::core::application {

public:

  application();

  ~application() override = default;

  auto update() -> void override;

  auto fixed_update() -> void override;

private:

  // auto _generate_icosphere(const std::float_t radius, const std::uint32_t subdivisions) -> std::unique_ptr<sbx::models::mesh>;

  std::unordered_map<sbx::utility::hashed_string, sbx::math::uuid> _texture_ids;
  std::unordered_map<sbx::utility::hashed_string, sbx::math::uuid> _mesh_ids;

  // sbx::units::second _time;
  // std::uint32_t _frames;

  // sbx::memory::observer_ptr<sbx::ui::label> _fps_label;
  // sbx::memory::observer_ptr<sbx::ui::label> _delta_time_label;

  sbx::math::angle _rotation;

  camera_controller _camera_controller;

}; // class application

} // namespace demo

#endif // DEMO_APPLICATION_HPP_
