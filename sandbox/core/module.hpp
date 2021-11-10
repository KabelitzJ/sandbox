#ifndef SBX_CORE_MODULE_HPP_
#define SBX_CORE_MODULE_HPP_

#include <vector>
#include <memory>
#include <string>
#include <cassert>

#include "scheduler.hpp"
#include "event_queue.hpp"

namespace sbx {

class module {

public:

  module() = default;
  virtual ~module() = default;

  virtual void initialize() = 0;
  virtual void terminate() = 0;

protected:

  template<typename System, typename... Args>
  void add_system(Args&&... args) {
    assert(_scheduler); // Scheduler is uninitialized
    _scheduler->add_system<System>(std::forward<Args>(args)...);
  }

  template<typename Function>
  void add_system(Function&& function) {
    assert(_scheduler); // Scheduler is uninitialized
    _scheduler->add_system(std::move(function));
  }

  template<typename Event, typename Listener>
  void add_listener(Listener&& listener) {
    assert(_event_queue); // Event queue is uninitialized
    _event_queue->add_listener<Event>(std::move(listener));
  }

  template<typename Event, typename... Args>
  void dispatch_event(Args&&... args) {
    assert(_event_queue); // Event queue is uninitialized
    _event_queue->dispatch_event<Event>(std::forward<Args>(args)...);
  }

  scheduler* get_scheduler() noexcept {
    assert(_scheduler); // Scheduler is uninitialized
    return _scheduler;
  }

  event_queue* get_event_queue() noexcept {
    assert(_event_queue); // Event queue is uninitialized
    return _event_queue;
  }

private:

  friend class engine;

  inline static scheduler* _scheduler{nullptr};
  inline static event_queue* _event_queue{nullptr};
  
}; // class module

} // namespace sbx

#endif // SBX_CORE_MODULE_HPP_
