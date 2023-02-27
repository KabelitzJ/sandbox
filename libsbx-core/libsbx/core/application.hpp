#ifndef LIBSBX_CORE_APPLICATION_HPP_
#define LIBSBX_CORE_APPLICATION_HPP_

#include <cinttypes>
#include <utility>

#include <libsbx/core/version.hpp>

#include <libsbx/utility/observer_ptr.hpp>

namespace sbx::core {

class engine;

class application {

public:

  application(utility::observer_ptr<engine> engine) noexcept
  : _engine{engine} {}

  virtual ~application() = default;

  virtual auto update() -> void = 0;

protected:

  utility::observer_ptr<engine> _engine{};

}; // class application

} // namespace sbx::core

#endif // LIBSBX_CORE_APPLICATION_HPP_
