#ifndef SBX_CORE_MODULE_HPP_
#define SBX_CORE_MODULE_HPP_

#include <vector>
#include <memory>

#include <ecs/registry.hpp>

#include "scheduler.hpp"
#include "event_queue.hpp"
#include "logger.hpp"

namespace sbx {

struct module_base {

  virtual ~module_base() = default;

  virtual void initialize() = 0;
  virtual void terminate() = 0;  

}; // struct module_base

class module : public module_base {

public:
  module() = default;
  virtual ~module() = default;

protected:
  static registry* _registry;
  static scheduler* _scheduler;
  static event_queue* _event_queue;
  static logger* _logger;
  
private:
  friend class engine;
  
}; // class module

} // namespace sbx

#endif // SBX_CORE_MODULE_HPP_
