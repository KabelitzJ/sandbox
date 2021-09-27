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
  using delta_type = Delta;

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

  template<typename Process, typename... Args>
  void attach(Args&&... args) {
    static_assert(std::is_base_of_v<basic_process<Process, delta_type>, Process>, "Invalid process type");
    
    auto process = typename process_handler::instance_type{new Process{std::forward<Args>(args)...}, &scheduler::deleter<Process>};
    auto handler = process_handler{std::move(process), &scheduler::update<Process>, &scheduler::abort<Process>};
  
    handler.update(handler, delta_type{});

    handlers.push_back(std::move(handler));
  }

  template<typename Function>
  void attach(Function&& function) {
    using Process = process_adaptor<std::decay_t<Function>, delta_type>;
    
    attach<Process>(std::forward<Function>(function));
  }

  void update(const delta_type delta) {
    auto size = handlers.size();

    for (auto position = handlers.size(); position; --position) {
      auto& handler = handlers.at(position - 1);

      if (const auto dead = handler.update(handler, delta); dead) {
        std::swap(handler, handlers.at(--size));
      }
    }

    handlers.erase(handlers.begin() + size, handlers.end());
  }

  void abort(const bool immediate = false) {
    auto exec = decltype(handlers){};
    exec.swap(handlers);

    for (auto&& handler : exec) {
      handler.abort(handler, immediate);
    }

    std::move(handlers.begin(), handlers.end(), std::back_inserter(exec));
    handlers.swap(exec);
  }

private:

  struct process_handler {
    using instance_type = std::unique_ptr<void, void(*)(void*)>;
    using update_fn_type = bool(process_handler&, const delta_type);
    using abort_fn_type = void(process_handler&, bool);

    instance_type instance;
    update_fn_type* update_fn;
    abort_fn_type* abort_fn;
  }; // struct process_handler

  template<typename Process>
  [[nodiscard]] static bool update(process_handler& handler, const delta_type delta) {
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
