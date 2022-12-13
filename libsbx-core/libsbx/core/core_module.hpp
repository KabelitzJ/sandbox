#ifndef LIBSBX_CORE_CORE_MODULE_HPP_
#define LIBSBX_CORE_CORE_MODULE_HPP_

#include  <libsbx/core/module.hpp>
#include  <libsbx/core/dispatcher.hpp>

namespace sbx::core {

class core_module : public module<core_module> {

  inline static const auto registered = register_module(stage::normal);

public:

  core_module() = default;

  ~core_module() override = default;

  void update([[maybe_unused]] const core::time& delta_time) override {
    _dispatcher.emit_all();
  }

  dispatcher& dispatcher() {
    return _dispatcher;
  }

private:

  core::dispatcher _dispatcher{};

}; // class core_module

} // namespace sbx::core

#endif // LIBSBX_CORE_CORE_MODULE_HPP_
