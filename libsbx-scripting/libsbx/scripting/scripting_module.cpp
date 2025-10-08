#include <libsbx/scripting/scripting_module.hpp>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

namespace sbx::scripting {

scripting_module::scripting_module() {

}

scripting_module::~scripting_module() {

}

auto scripting_module::update() -> void {
  SBX_SCOPED_TIMER("scripting_module");


}

auto scripting_module::instantiate(const scenes::node node, const std::string& name) -> void {

}

} // namespace sbx::scripting