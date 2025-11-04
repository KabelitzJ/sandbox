#ifndef LIBSBX_SCRIPTING_TYPE_CACHE_HPP_
#define LIBSBX_SCRIPTING_TYPE_CACHE_HPP_

#include <unordered_map>
#include <string>

#include <libsbx/scripting/core.hpp>
#include <libsbx/scripting/type.hpp>
#include <libsbx/scripting/stable_vector.hpp>

namespace sbx::scripting::detail {

class type_cache {

public:

  static auto get() -> type_cache&;

  auto cache_type(type&& InType) -> type*;

  auto get_type_by_name(std::string_view name) const -> type*;

  auto get_type_by_id(type_id id) const -> type*;

  auto clear() -> void;

private:

  stable_vector<type> _types;
  std::unordered_map<std::string, type*> _name_cache;
  std::unordered_map<type_id, type*> _id_cache;

}; // class type_cache

} // namespace sbx::scripting::detail

#endif // LIBSBX_SCRIPTING_TYPE_CACHE_HPP_