#ifndef SBX_CORE_ENGINE_HPP_
#define SBX_CORE_ENGINE_HPP_

#include <memory>
#include <vector>

#include <types/primitives.hpp>

#include <utils/type_id.hpp>

#include "scene.hpp"
#include "scheduler.hpp"
#include "event_queue.hpp"
#include "module.hpp"
#include "resource_cache.hpp"

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

    _modules.emplace_back(std::move(module));
  }

private:
  std::unique_ptr<scene> _scene{};
  std::unique_ptr<scheduler> _scheduler{};
  std::unique_ptr<event_queue> _event_queue{};
  std::unique_ptr<resource_cache> _resource_cache{};
  std::vector<std::unique_ptr<module>> _modules{};

}; // class engine


} // namespace sbx

#endif // SBX_CORE_ENGINE_HPP_
