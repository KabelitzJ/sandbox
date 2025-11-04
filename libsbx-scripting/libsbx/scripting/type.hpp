#ifndef LIBSBX_SCRIPTING_TYPE_HPP_
#define LIBSBX_SCRIPTING_TYPE_HPP_

#include <libsbx/scripting/core.hpp>
#include <libsbx/scripting/string.hpp>
#include <libsbx/scripting/object.hpp>

namespace sbx::scripting {

class type {

  friend class host_instance;
  friend class assembly;
  friend class assembly_load_context;
  friend class method_info;
  friend class field_info;
  friend class property_info;
  friend class attribute;
  friend class reflection_type;
  friend class object;

public:

  auto get_full_name() const -> string;
  
  auto get_type_id() const -> type_id;

  template<typename... Args>
  auto create_instance(Args&&... args) const -> object {
    constexpr auto argument_count = sizeof...(args);

    auto result = object{};

    if constexpr (argument_count > 0) {
      const auto arguments = std::array<void*, argument_count>{};
      auto argument_types = std::array<managed_type, argument_count>{};

      add_to_array<Args...>(arguments, argument_types, std::forward<Args>(args)..., std::make_index_sequence<argument_count>{});

      result = _create_instance_internal(arguments.data(), argument_types.data(), argument_count);
    } else {
      result = _create_instance_internal(nullptr, nullptr, 0);
    }

    return result;
  }

private:

  auto _create_instance_internal(const void** parameters, const managed_type* parameter_types, std::size_t length) const -> object;

  type_id _id = -1;
  type* _base_type = nullptr;
  type* _element_type = nullptr;

}; // class type

} // namespace sbx::scripting  

#endif // LIBSBX_SCRIPTING_TYPE_HPP_