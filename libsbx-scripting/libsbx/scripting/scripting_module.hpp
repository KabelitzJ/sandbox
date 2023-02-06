#ifndef LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
#define LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_

#include <memory>

#include <libsbx/core/module.hpp>

#include <libsbx/async/async_module.hpp>

namespace sbx::scripting {

class scripting_module : public core::module<scripting_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<async::async_module>{});

public:

  scripting_module() {

  }

  ~scripting_module() override {

  }

  auto update([[maybe_unused]] std::float_t delta_time) -> void override {

  }

private:



}; // class scripting_module

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
