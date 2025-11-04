#include <libsbx/scripting/garbage_collection.hpp>

#include <functional>

#include <libsbx/scripting/detail/backend.hpp>

namespace sbx::scripting {

auto garbage_collection::collect() -> void {
  collect(-1, mode::standart, true, false);
}

auto garbage_collection::collect(std::int32_t generation, mode collection_mode, bool is_blocking, bool compacting) -> void {
  std::invoke(detail::backend.collect_garbage, generation, collection_mode, is_blocking, compacting);
}

auto garbage_collection::wait_for_pending_finalizers() -> void {
  std::invoke(detail::backend.wait_for_pending_finalizers);
}

}; // namespace sbx::scripting