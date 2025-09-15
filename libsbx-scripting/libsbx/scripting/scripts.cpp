#include <libsbx/scripting/scripts.hpp>

#include <libsbx/scripting/scripting_module.hpp>

namespace sbx::scripting {

auto scripts::add(const std::string& script) -> void {
  auto& scripting_module = core::engine::get_module<scripting::scripting_module>();

  scripting_module.instantiate(_node, script);
}

} // namespace sbx::scripting