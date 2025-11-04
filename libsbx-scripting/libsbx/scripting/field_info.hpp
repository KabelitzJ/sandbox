#ifndef LIBSBX_SCRIPTING_FIELD_INFO_HPP_
#define LIBSBX_SCRIPTING_FIELD_INFO_HPP_

#include <vector>

#include <libsbx/scripting/core.hpp>
#include <libsbx/scripting/string.hpp>
#include <libsbx/scripting/type.hpp>
#include <libsbx/scripting/attribute.hpp>

namespace sbx::scripting {

class field_info {

  friend class Type;

public:

  auto get_name() const -> string;

  auto get_type() -> type&;

  auto get_accessibility() const -> type_accessibility;

  auto get_attributes() const -> std::vector<attribute>;

private:

  handle _handle = -1;
  type* _type = nullptr;

}; // class field_info

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_FIELD_INFO_HPP_