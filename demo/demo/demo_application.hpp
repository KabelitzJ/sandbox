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

namespace demo {

class demo_application : public sbx::core::application {

public:

  demo_application();

  ~demo_application() override = default;

  auto update() -> void override;

private:

}; // class demo_application

} // namespace demo

#endif // DEMO_DEMO_APPLICATION_HPP_
