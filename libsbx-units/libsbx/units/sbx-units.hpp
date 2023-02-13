#pragma once

#include <iosfwd>
#include <string>

#include <libsbx/units/export.hpp>

namespace sbx_units
{
  // Print a greeting for the specified name into the specified
  // stream. Throw std::invalid_argument if the name is empty.
  //
  LIBSBX_UNITS_SYMEXPORT void
  say_hello (std::ostream&, const std::string& name);
}
