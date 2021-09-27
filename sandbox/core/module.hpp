#ifndef SBX_CORE_MODULE_HPP_
#define SBX_CORE_MODULE_HPP_

#include <vector>
#include <memory>

#include <ecs/registry.hpp>
#include <ecs/system.hpp>
#include <ecs/scheduler.hpp>

namespace sbx {

class module {

public:
  module();
  virtual ~module() = default;

  virtual void initialize() = 0;

protected:
  template<typename System, typename... Args>
  void add_system(Args&&... args);

  registry* _registry{};
  scheduler<fast_time>* _scheduler{};
  
private:
  void _initialize();

  std::vector<std::unique_ptr<system>> _systems{};

  friend class engine;

}; // class module

template<typename System, typename... Args>
inline void module::add_system(Args&&... args) {
  static_assert(std::is_base_of_v<system, System>);
  static_assert(!std::is_abstract_v<System>);

  auto system = std::make_unique<System>(std::forward<Args>(args)...);
  system->_registry = _registry;
  system->_scheduler = _scheduler;

  _systems.push_back(std::move(system));
}

} // namespace sbx

#endif // SBX_CORE_MODULE_HPP_
