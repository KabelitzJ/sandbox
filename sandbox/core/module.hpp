#ifndef SBX_CORE_MODULE_HPP_
#define SBX_CORE_MODULE_HPP_

#include <vector>
#include <memory>

#include <ecs/scheduler.hpp>
#include <ecs/event_queue.hpp>

namespace sbx {

class module {

public:
  module();
  virtual ~module() = default;

  virtual void initialize() = 0;

  virtual void terminate() = 0;

protected:
  template<typename System, typename... Args>
  void add_system(Args&&... args) {
    static_assert(std::is_base_of_v<basic_system<System, fast_time>, System>);
    static_assert(!std::is_abstract_v<System>);

    _scheduler->attach<System>(std::forward<Args>(args)...);
  }
  
  template<typename Event, typename Listener>
  void add_listener(Listener&& listener) {
     _event_queue->add_listener<Event>(std::forward<Listener>(listener));
  }
  
  template<typename Event, typename... Args>
  void emplace_event(Args&&... args) {
     _event_queue->emplace<Event>(std::forward<Args>(args)...);
  }
  
private:
  friend class engine;

  static scheduler* _scheduler;
  static event_queue* _event_queue;
  
}; // class module

} // namespace sbx

#endif // SBX_CORE_MODULE_HPP_
