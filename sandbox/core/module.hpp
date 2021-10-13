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

    scheduler->attach<System>(std::forward<Args>(args)...);
  }

  static ::sbx::scheduler* scheduler;
  static ::sbx::event_queue* event_queue;
  
private:
  friend class engine;

}; // class module

} // namespace sbx

#endif // SBX_CORE_MODULE_HPP_
