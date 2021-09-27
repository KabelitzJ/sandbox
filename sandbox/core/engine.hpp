#ifndef SBX_CORE_ENGINE_HPP_
#define SBX_CORE_ENGINE_HPP_

#include <vector>
#include <memory>

#include <ecs/registry.hpp>
#include <ecs/scheduler.hpp>

#include "module.hpp"

namespace sbx {

class engine {

public:
  engine();
  ~engine();

  void initialize();

  void start();

  template<typename Module, typename... Args>
  void add_module(Args&&... args);

private:
  std::unique_ptr<registry> _registry{};
  std::unique_ptr<scheduler<fast_time>> _scheduler{};
  std::vector<std::unique_ptr<module>> _modules{};

}; // class engine

template<typename Module, typename... Args>
void engine::add_module(Args&&... args) {
  static_assert(std::is_base_of_v<module, Module>);
  static_assert(!std::is_abstract_v<Module>);

  auto module = std::make_unique<Module>(std::forward<Args>(args)...);
  module->_registry = _registry.get();
  module->_scheduler = _scheduler.get(); 
  
  _modules.push_back(std::move(module));
}

} // namespace sbx

#endif // SBX_CORE_ENGINE_HPP_
