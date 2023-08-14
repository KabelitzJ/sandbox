#ifndef LIBSBX_CORE_ENTRY_POINT_HPP_
#define LIBSBX_CORE_ENTRY_POINT_HPP_

#include <memory>

#include <libsbx/core/application.hpp>

namespace sbx::core {

extern auto create_application() -> std::unique_ptr<application>;

} // namespace sbx::core

#endif // LIBSBX_CORE_ENTRY_POINT_HPP_
