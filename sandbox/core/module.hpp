#ifndef SBX_CORE_MODULE_HPP_
#define SBX_CORE_MODULE_HPP_

#include <vector>
#include <memory>
#include <string>

#include "user.hpp"

namespace sbx {

class module : public event_queue_user, public resource_cache_user, public scene_user, public scheduler_user {

public:

  module() = default;
  virtual ~module() = default;

  virtual void initialize() = 0;
  virtual void terminate() = 0;  
  
}; // class module

} // namespace sbx

#endif // SBX_CORE_MODULE_HPP_
