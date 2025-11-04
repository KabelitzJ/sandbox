#ifndef LIBSBX_SCRIPTING_MEMORY_HPP_
#define LIBSBX_SCRIPTING_MEMORY_HPP_

#include <libsbx/scripting/core.hpp>

namespace sbx::scripting {

struct memory {

  static auto allocate_hglobal(std::size_t size) -> void*;
  static auto free_hglobal(void* pointer) -> void;

  static auto string_to_co_task_memory_auto(string_view_type string) -> char_type*;
  static auto free_co_task_memory(void* memory) -> void;

}; // struct memory

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_MEMORY_HPP_