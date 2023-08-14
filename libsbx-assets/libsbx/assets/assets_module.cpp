#include <libsbx/assets/assets_module.hpp>

#include <libsbx/core/engine.hpp>

// #include <libsbx/graphics/graphics_module.hpp>

namespace sbx::assets {

assets_module::assets_module() {
  
}

assets_module::~assets_module() {
  unload_assets();
}

auto assets_module::update() -> void {

}

} // namespace sbx::assets
