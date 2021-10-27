#ifndef SBX_CORE_ENGINE_HPP_
#define SBX_CORE_ENGINE_HPP_

#include <unordered_map>
#include <memory>

#include <types/primitives.hpp>

#include <utils/type_id.hpp>

#include <ecs/registry.hpp>

#include "scheduler.hpp"
#include "event_queue.hpp"
#include "module.hpp"
#include "logger.hpp"

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
    
    constexpr auto id = type_id<Module>{};

    auto module = std::make_unique<Module>(std::forward<Args>(args)...);
    
    _modules.emplace(id, std::move(module));
  }

private:
  std::unique_ptr<registry> _registry{};
  std::unique_ptr<scheduler> _scheduler{};
  std::unique_ptr<event_queue> _event_queue{};
  std::unique_ptr<logger> _logger{};
  std::unordered_map<uint32, std::unique_ptr<module>> _modules{};

}; // class engine


} // namespace sbx

#endif // SBX_CORE_ENGINE_HPP_
