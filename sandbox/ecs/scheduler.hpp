#ifndef SBX_ECS_BASIC_SCHEDULER_HPP_
#define SBX_ECS_BASIC_SCHEDULER_HPP_

#include <memory>
#include <vector>

#include "process.hpp"

namespace sbx {

template<typename>
class basic_scheduler;

using scheduler = basic_scheduler<fast_time>;
  
template<typename Delta>
class basic_scheduler {

  struct process_handler {
    using instance_type = std::unique_ptr<void, void(*)(void*)>;
    using update_fn_type = bool(process_handler&, const Delta);
    using abort_fn_type = void(process_handler&, bool);

    instance_type instance;
    update_fn_type* update;
    abort_fn_type* abort;
  }; // struct process_handler

public:

  using size_type = std::size_t;
  using delta_type = Delta;

  basic_scheduler() = default;
  ~basic_scheduler() = default;

  [[nodiscard]] size_type size() const noexcept {
    return _handlers.size();
  }

  [[nodiscard]] bool is_empty() const noexcept {
    return _handlers.empty();
  }

  void clear() {
    _handlers.clear();
  }

  template<typename Process, typename... Args>
  void attach(Args&&... args) {
    static_assert(std::is_base_of_v<basic_process<Process, delta_type>, Process>, "Invalid process type");
    
    auto process = typename process_handler::instance_type{new Process{std::forward<Args>(args)...}, &basic_scheduler::_deleter<Process>};
    auto handler = process_handler{std::move(process), &basic_scheduler::_update<Process>, &basic_scheduler::_abort<Process>};
  
    handler.update(handler, delta_type{});

    _handlers.push_back(std::move(handler));
  }

  template<typename Function>
  void attach(Function&& function) {
    using Process = process_adaptor<std::decay_t<Function>, delta_type>;
    
    attach<Process>(std::forward<Function>(function));
  }

  void update(const delta_type delta) {
    _handlers.erase(
      std::remove_if(_handlers.begin(), _handlers.end(), [&](auto& hander){
        return hander.update(hander, delta);
      }),
      _handlers.end()
    );
  }

  void abort(const bool immediate = false) {
    auto exec = decltype(_handlers){_handlers.size()};
    exec.swap(_handlers);

    for (auto&& handler : exec) {
      handler.abort(handler, immediate);
    }

    std::move(_handlers.begin(), _handlers.end(), std::back_inserter(exec));
    _handlers.swap(exec);
  }

private:

  /**
   * @brief 
   * 
   * @tparam Process 
   * @param handler 
   * @param delta 
   * @return true When the process has either succeeded or been aborted
   * @return false When the process is still running
   */
  template<typename Process>
  [[nodiscard]] static bool _update(process_handler& handler, const delta_type delta) {
    auto* process = static_cast<Process*>(handler.instance.get());
    process->tick(delta);

    return process->is_finished();
  }

  template<typename Process>
  static void _abort(process_handler& handler, const bool immediate) {
    static_cast<Process*>(handler.instance.get())->abort(immediate);
  }

  template<typename Process>
  static void _deleter(void* process) {
    delete static_cast<Process*>(process);
  }

  std::vector<process_handler> _handlers{};

}; // class basic_scheduler
  
} // namespace sbx

#endif // SBX_ECS_BASIC_SCHEDULER_HPP_
