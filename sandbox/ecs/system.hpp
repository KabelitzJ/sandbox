#ifndef SBX_ECS_SYSTEM_HPP_
#define SBX_ECS_SYSTEM_HPP_

#include "registry.hpp"

namespace sbx {

class system {

public:
  system();
  virtual ~system() = default;

  virtual void initialize() = 0;

protected:
  registry* _registry;

private:

  friend class module;

}; // class system

} // namespace sbx

#endif // SBX_ECS_SYSTEM_HPP_
