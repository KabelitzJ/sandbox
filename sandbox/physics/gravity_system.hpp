#ifndef SBX_PHYSICS_GRAVITY_SYSTEM_HPP_
#define SBX_PHYSICS_GRAVITY_SYSTEM_HPP_

#include <core/system.hpp>

namespace sbx {

class gravity_system final : public system {

public:

  gravity_system(scene* scene);
  ~gravity_system() = default;

  void initialize() override;
  void update(const time delta_time) override;
  void terminate() override;

private:

  scene* _scene{};

}; // class gravity_system

} // namespace sbx

#endif // SBX_PHYSICS_GRAVITY_SYSTEM_HPP_
