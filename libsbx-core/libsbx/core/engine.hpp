#ifndef LIBSBX_CORE_ENGINE_HPP_
#define LIBSBX_CORE_ENGINE_HPP_

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include <vector>
#include <string_view>
#include <cmath>
#include <chrono>

#include <libsbx/core/module.hpp>
#include <libsbx/core/application.hpp>

namespace sbx::core {

class engine {

  using stage = module_manager::stage;
  using module_base = module_manager::module_base;
  using module_factory = module_manager::module_factory;

public:

  engine(std::vector<std::string_view>&& args)
  : _args{std::move(args)} {
    _create_modules();
  }

  ~engine() {
    _destroy_modules();
  }

  auto run(std::unique_ptr<application>& application) -> void {
    using clock_type = std::chrono::high_resolution_clock;

    _is_running = true;

    auto last = clock_type::now();

    while (_is_running) {
      const auto now = clock_type::now();
      const auto delta_time = std::chrono::duration_cast<std::chrono::duration<std::float_t>>(last - now).count();
      last = now;

      _update_stage(stage::pre, delta_time);
      _update_stage(stage::normal, delta_time);
      _update_stage(stage::post, delta_time);
    }
  }

  auto quit() -> void {
    _is_running = false;
  }

private:

  auto _create_modules() -> void {
    for (const auto& [type, factory] : module_manager::_factories) {
      _create_module(type, factory);
    }
  }

  auto _create_module(std::type_index type, const module_factory& factory) -> void {
    if (_modules.contains(type)) {
      return;
    }

    for (const auto& dependency : factory.dependencies) {
      _create_module(dependency, module_manager::_factories.at(dependency));
    }

    _modules.insert({type, factory.create()});
    _module_by_stage[factory.stage].push_back(type);
  }

  auto _destroy_modules() -> void {
    for (const auto& entry : _modules) {
      _destroy_module(entry.first);
    }
  }

  auto _destroy_module(std::type_index type) -> void {
    if (auto entry = _modules.find(type); entry != _modules.cend()) {
      for (const auto& dependency : module_manager::_factories.at(type).dependencies) {
        _destroy_module(dependency);
      }

      entry->second.reset();
    }
  }

  auto _update_stage(stage stage, std::float_t delta_time) -> void {
    if (auto entry = _module_by_stage.find(stage); entry != _module_by_stage.end()) {
      for (const auto& type : entry->second) {
        _modules.at(type)->update(delta_time);
      }
    }
  }

  bool _is_running{};
  std::vector<std::string_view> _args{};

  std::unordered_map<std::type_index, std::unique_ptr<module_base>> _modules{};
  std::unordered_map<stage, std::vector<std::type_index>> _module_by_stage{};

}; // class engine

} // namespace sbx::core
#endif // LIBSBX_CORE_ENGINE_HPP_
