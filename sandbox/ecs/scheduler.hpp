#ifndef SBX_ECS_SCHEDULER_HPP_
#define SBX_ECS_SCHEDULER_HPP_

#include <memory>
#include <utility>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <iostream>

#include <types/primitives.hpp>

#include "system.hpp"

namespace sbx {

template<typename Delta>
class basic_scheduler;

using scheduler = basic_scheduler<time>;

template<typename Delta>
class basic_scheduler {

  struct system_handle {
    using instance_type = std::unique_ptr<void, void(*)(void*)>;
    using update_fn_type = bool(system_handle&, Delta);
    using abort_fn_type = void(system_handle&, const bool);

    instance_type instance;
    update_fn_type* update;
    abort_fn_type* abort;
  }; // struct system_handle

public:
  using delta_type = Delta;
  using size_type = std::size_t;

  basic_scheduler()
  : _handlers{} { }

  basic_scheduler(const basic_scheduler&) = delete;

  basic_scheduler(basic_scheduler&&) = default;

  basic_scheduler& operator=(const basic_scheduler&) = delete;

  basic_scheduler& operator=(basic_scheduler&&) = default;

  ~basic_scheduler() {
    abort(true);
    clear();
  }

  [[nodiscard]] size_type size() const noexcept {
    return _handlers.size();
  }

  [[nodiscard]] bool is_empty() const noexcept {
    return _handlers.empty();
  }

  void clear() {
    _handlers.clear();
  }

  template<typename System, typename... Args>
  void attach(Args&&... args) {
    static_assert(std::is_base_of_v<basic_system<System, delta_type>, System>, "Invalid system type");

    auto system = typename system_handle::instance_type{new System{std::forward<Args>(args)...}, [](auto* ptr){ delete static_cast<System*>(ptr); }};

    auto handle = system_handle{std::move(system), &basic_scheduler::_update<System>, &basic_scheduler::_abort<System>};

    handle.update(handle, delta_type{});

    _handlers.emplace_back(std::move(handle));
  }

  template<typename Function>
  void attach(Function&& function) {
    attach<system_adaptor<std::decay_t<Function>, delta_type>>(std::forward<Function>(function));
  }

  void update(const delta_type delta_time) {
    _handlers.erase(
      std::remove_if(
        _handlers.begin(),
        _handlers.end(),
        [&](auto& handle){ return handle.update(handle, delta_time); }
      ),
      _handlers.end()
    );
  }

  void abort(const bool immediately = false) {
    auto executors = decltype(_handlers){};
    executors.swap(_handlers);

    for (auto&& handle : executors) {
      handle.abort(handle, immediately);
    }

    std::move(_handlers.begin(), _handlers.end(), std::back_inserter(executors));
    _handlers.swap(executors);
  }

private:
  template<typename System>
  [[nodiscard]] static bool _update(system_handle& handle, const delta_type delta_time) {
    auto* system = static_cast<System*>(handle.instance.get());
    system->tick(delta_time);

    return system->is_finished();
  }

  template<typename System>
  static void _abort(system_handle& handle, const bool immediately) {
    static_cast<System*>(handle.instance.get())->abort(immediately);
  }

  std::vector<system_handle> _handlers{};

}; // class basic_scheduler

} // namespace sbx

#endif // SBX_ECS_SCHEDULER_HPP_
