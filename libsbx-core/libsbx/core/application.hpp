#ifndef LIBSBX_CORE_APPLICATION_HPP_
#define LIBSBX_CORE_APPLICATION_HPP_

#include <cinttypes>
#include <utility>

#include <libsbx/core/version.hpp>

#include <libsbx/utility/ptr_view.hpp>

namespace sbx::core {

class engine;

class application {

public:

  application(utility::ptr_view<engine> engine) noexcept
  : _engine{std::move(engine)} {}

  virtual ~application() = default;

  virtual auto update() -> void = 0;

protected:

  utility::ptr_view<engine> _engine{};

}; // class application

} // namespace sbx::core

#endif // LIBSBX_CORE_APPLICATION_HPP_
