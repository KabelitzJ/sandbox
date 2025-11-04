#ifndef LIBSBX_SCRIPTING_METHOD_INFO_HPP_
#define LIBSBX_SCRIPTING_METHOD_INFO_HPP_

#include <vector>

#include <libsbx/scripting/core.hpp>
#include <libsbx/scripting/string.hpp>
#include <libsbx/scripting/type.hpp>
#include <libsbx/scripting/attribute.hpp>

namespace sbx::scripting {

class method_info {

  friend class type;

public:

  auto get_name() const -> string;

  auto get_return_type() -> type&;

  auto get_parameter_types() -> const std::vector<type*>&;

  auto get_accessibility() const -> type_accessibility;

  auto get_attributes() const -> std::vector<attribute>;

private:

  handle _handle = -1;
  type* _return_type = nullptr;
  std::vector<type*> _parameter_types;

}; // class method_info

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_METHOD_INFO_HPP_