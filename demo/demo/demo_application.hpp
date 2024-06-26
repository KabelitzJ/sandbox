#ifndef DEMO_DEMO_APPLICATION_HPP_
#define DEMO_DEMO_APPLICATION_HPP_

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

namespace demo {

class demo_application : public sbx::core::application {

public:

  demo_application();

  ~demo_application() override = default;

  auto update() -> void override;

private:

  std::vector<sbx::math::uuid> _texture_ids;
  std::vector<sbx::math::uuid> _mesh_ids;

  sbx::math::uuid _plane_id;
  sbx::math::uuid _sphere_id;
  std::vector<sbx::math::uuid> _monkey_ids;

  sbx::units::second _time;
  std::uint32_t _frames;

  sbx::memory::observer_ptr<sbx::ui::label> _fps_label;
  sbx::memory::observer_ptr<sbx::ui::label> _delta_time_label;

  sbx::math::angle _rotation;

  sbx::math::angle _orbit_angle;
  sbx::math::angle _tilt_angle;
  sbx::math::vector3 _target;
  std::float_t _zoom;

}; // class demo_application

} // namespace demo

#endif // DEMO_DEMO_APPLICATION_HPP_
