#include <libsbx/core/application.hpp>

#include <libsbx/core/engine.hpp>

namespace sbx::core {

auto application::quit() noexcept -> void {
  _engine->quit();
}

auto application::_set_engine(utility::observer_ptr<engine> engine) noexcept -> void {
  _engine = engine;
}

} // namespace sbx::core
