#ifndef SBX_CORE_MODULE_HPP_
#define SBX_CORE_MODULE_HPP_

#include <vector>
#include <memory>

#include "scene.hpp"
#include "scheduler.hpp"
#include "event_queue.hpp"
#include "resource_cache.hpp"

namespace sbx {

class module {

public:

  module() = default;
  virtual ~module() = default;

  virtual void initialize() = 0;
  virtual void terminate() = 0;  

protected:

  inline static scene* _scene{nullptr};
  inline static scheduler* _scheduler{nullptr};
  inline static event_queue* _event_queue{nullptr};
  inline static resource_cache* _resource_cache{nullptr};
  
private:

  friend class engine;
  
}; // class module

} // namespace sbx

#endif // SBX_CORE_MODULE_HPP_
