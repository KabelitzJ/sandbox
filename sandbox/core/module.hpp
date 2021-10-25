#ifndef SBX_CORE_MODULE_HPP_
#define SBX_CORE_MODULE_HPP_

#include <vector>
#include <memory>

#include <ecs/registry.hpp>

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
  static registry* _registry;
  static scheduler* _scheduler;
  static event_queue* _event_queue;
  
private:
  friend class engine;
  
}; // class module

} // namespace sbx

#endif // SBX_CORE_MODULE_HPP_
