#ifndef SBX_ECS_SCHEDULER_HPP_
#define SBX_ECS_SCHEDULER_HPP_

#include <memory>
#include <utility>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <iostream>
#include <chrono>

#include <types/primitives.hpp>

#include "system.hpp"

namespace sbx {

class scheduler {

public:

  scheduler()
  : _systems{} { }

  scheduler(const scheduler&) = delete;

  scheduler(scheduler&&) = default;

  scheduler& operator=(const scheduler&) = delete;

  scheduler& operator=(scheduler&&) = default;

  ~scheduler() {
    abort();
    clear();
  }

  [[nodiscard]] bool is_empty() const noexcept {
    return _systems.empty();
  }

  void clear() {
    _systems.clear();
  }

  template<typename System, typename... Args>
  void add_system(Args&&... args) {
    static_assert(!std::is_abstract_v<System>, "System can not be abstract");
    static_assert(std::is_constructible_v<System, Args...>, "System must be constructable by given arguments");
    static_assert(std::is_base_of_v<system, System>, "Invalid system type");

    auto system = std::make_unique<System>(std::forward<Args>(args)...);

    system->_initialize();

    _systems.emplace_back(std::move(system));
  }

  template<typename Function>
  void add_system(Function&& function) {
    static_assert(std::is_invocable_r_v<void, Function, const time, void(*)(void)>, "Function has wrong signature");
    add_system<system_adaptor<std::decay_t<Function>>>(std::forward<Function>(function));
  }

  void update(const time delta_time) {
    _systems.erase(
      std::remove_if(
        _systems.begin(),
        _systems.end(),
        [&](auto& system){ 
          system->_update(delta_time);
          return system->is_finished();
        }
      ),
      _systems.end()
    );
  }

  void abort() {
    for (auto& system : _systems) {
      system->abort();
    }
  }

private:

  std::vector<std::unique_ptr<system>> _systems{};

}; // class basic_scheduler

} // namespace sbx

#endif // SBX_ECS_SCHEDULER_HPP_
