#include <libsbx/assets/assets_module.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::assets {

assets_module::assets_module() {
  
}

assets_module::~assets_module() {
  // [NOTE] KAJ 2023-07-31 : This kind of seems like a cyclic dependency. Need to figure out how to break it. 
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  auto& logical_device = graphics_module.logical_device();

  // [NOTE] KAJ 2023-07-31 : Wait for all gpu operations to finish before unloading assets.
  logical_device.wait_idle();

  unload_assets();
}

auto assets_module::update() -> void {

}

} // namespace sbx::assets