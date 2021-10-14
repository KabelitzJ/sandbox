#ifndef SBX_CORE_ENGINE_HPP_
#define SBX_CORE_ENGINE_HPP_

#include <vector>
#include <memory>

#include <ecs/scheduler.hpp>
#include <ecs/event_queue.hpp>

#include "module.hpp"

namespace sbx {

class engine {

public:
  engine();
  ~engine();

  void initialize();

  void start();

  void terminate();

  template<typename Module, typename... Args>
  void add_module(Args&&... args) {
    static_assert(std::is_base_of_v<module, Module>);
    static_assert(!std::is_abstract_v<Module>);

    auto module = std::make_unique<Module>(std::forward<Args>(args)...);
    
    _modules.push_back(std::move(module));
  }

private:
  std::unique_ptr<scheduler> _scheduler{};
  std::unique_ptr<event_queue> _event_queue{};
  std::vector<std::unique_ptr<module>> _modules{};

}; // class engine


} // namespace sbx

#endif // SBX_CORE_ENGINE_HPP_
