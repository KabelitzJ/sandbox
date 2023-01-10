#ifndef LIBSBX_CORE_APPLICATION_HPP_
#define LIBSBX_CORE_APPLICATION_HPP_

#include <cinttypes>
#include <utility>

#include <libsbx/core/version.hpp>

namespace sbx::core {

class application {

public:

  virtual ~application() = default;

  virtual auto update() -> void = 0;

}; // class application

} // namespace sbx::core

#endif // LIBSBX_CORE_APPLICATION_HPP_
