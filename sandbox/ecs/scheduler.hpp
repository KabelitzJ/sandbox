#ifndef SBX_ECS_SCHEDULER_HPP_
#define SBX_ECS_SCHEDULER_HPP_

#include <memory>
#include <vector>

#include "process.hpp"

namespace sbx {
  
template<typename Delta>
class scheduler {

public:

  using size_type = std::size_t;

  scheduler() = default;
  ~scheduler() = default;

  [[nodiscard]] size_type size() const noexcept {
    return handlers.size();
  }

  [[nodiscard]] bool empty() const noexcept {
    return handlers.empty();
  }

  void clear() {
    handlers.clear();
  }

private:

  struct process_handler {
    using instance_type = std::unique_ptr<void, void(*)(void*)>;
    using update_fn_type = bool(process_handler&, const Delta);
    using abort_fn_type = void(process_handler&, bool);

    instance_type instance;
    update_fn_type* update_fn;
    abort_fn_type* abort_fn;
  }; // struct process_handler

  template<typename Process>
  [[nodiscard]] static bool update(process_handler& handler, const Delta delta) {
    auto* process = static_cast<Process*>(handler.instance.get());
    process->tick(delta);

    return process->is_finished();
  }

  template<typename Process>
  static void abort(process_handler& handler, const bool immediate) {
    static_cast<Process*>(handler.instance.get())->abort(immediate);
  }

  template<typename Process>
  static void deleter(void* process) {
    delete static_cast<Process*>(process);
  }

  std::vector<process_handler> handlers{};

}; // class scheduler
  
} // namespace sbx

#endif // SBX_ECS_SCHEDULER_HPP_
