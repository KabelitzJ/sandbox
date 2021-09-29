#ifndef SBX_ECS_SYSTEM_HPP_
#define SBX_ECS_SYSTEM_HPP_

#include "registry.hpp"
#include "scheduler.hpp"

namespace sbx {

class system {

public:
  system();
  virtual ~system() = default;

  virtual void initialize() = 0;

protected:
  basic_registry<entity>* _registry{};
  scheduler<fast_time>* _scheduler{};

private:

  friend class module;

}; // class system

} // namespace sbx

#endif // SBX_ECS_SYSTEM_HPP_
