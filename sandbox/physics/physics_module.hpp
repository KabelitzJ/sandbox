#ifndef SBX_PHYSICS_PHYSICS_MODULE_HPP_
#define SBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <core/module.hpp>

namespace sbx {

class physics_module final : public module {

public:

  physics_module();
  ~physics_module() = default;

  void initialize() override;
  void terminate() override;

private:

}; // class physics_module

} // namespace sbx

#endif // SBX_PHYSICS_PHYSICS_MODULE_HPP_
