#include <libsbx/scripting/managed/type.hpp>

#include <libsbx/scripting/managed/detail/backend.hpp>

namespace sbx::scripting::managed {

auto type::get_full_name() const -> string {
  return std::invoke(detail::backend.get_full_type_name, _id);
}
  
auto type::get_type_id() const -> type_id {
  return _id;
}

auto type::_create_instance_internal(const void** parameters, const managed_type* parameter_types, std::size_t length) const -> object {
  auto result = object{};

  result._handle = std::invoke(detail::backend.create_object, _id, false, parameters, parameter_types, static_cast<std::int32_t>(length));
  result._type = this;

  return result;
}

} // namespace sbx::scripting::managed