#ifndef DEMO_DEMO_APPLICATION_HPP_
#define DEMO_DEMO_APPLICATION_HPP_

#include <libsbx/units/units.hpp>
#include <libsbx/utility/utility.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/memory/memory.hpp>
#include <libsbx/signals/signals.hpp>
#include <libsbx/bitmaps/bitmaps.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/async/async.hpp>
#include <libsbx/assets/assets.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>
#include <libsbx/models/models.hpp>
#include <libsbx/scenes/scenes.hpp>
#include <libsbx/audio/audio.hpp>
#include <libsbx/ui/ui.hpp>
#include <libsbx/shadows/shadows.hpp>
#include <libsbx/physics/physics.hpp>

namespace demo {

class demo_application : public sbx::core::application {

public:

  demo_application();

  ~demo_application() override = default;

  auto update() -> void override;

private:

  sbx::memory::observer_ptr<sbx::ui::label> _label_fps;
  sbx::memory::observer_ptr<sbx::ui::label> _label_delta_time;

  // std::optional<sbx::scenes::node> _cube;
  // sbx::assets::asset_id _mesh_id;
  // sbx::assets::asset_id _texture_id;
  // bool _flag = false;

  sbx::units::second _time;
  std::uint32_t _frames;

}; // class demo_application

} // namespace demo

#endif // DEMO_DEMO_APPLICATION_HPP_
