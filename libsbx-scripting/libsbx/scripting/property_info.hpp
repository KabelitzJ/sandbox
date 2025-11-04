#ifndef LIBSBX_SCRIPTING_PROPERTY_INFO_HPP_
#define LIBSBX_SCRIPTING_PROPERTY_INFO_HPP_

#include <vector>

#include <libsbx/scripting/core.hpp>
#include <libsbx/scripting/string.hpp>
#include <libsbx/scripting/type.hpp>
#include <libsbx/scripting/attribute.hpp>

namespace sbx::scripting {

class property_info {

  friend class type;

public:

  auto get_name() const -> string;
  
  auto get_type() -> type&;

  auto get_attributes() const -> std::vector<attribute>;

private:

  handle _handle = -1;
  type* _type = nullptr;

}; // class property_info

} // sbx

#endif // LIBSBX_SCRIPTING_PROPERTY_INFO_HPP_