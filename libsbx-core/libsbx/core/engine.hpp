#ifndef LIBSBX_CORE_ENGINE_HPP_
#define LIBSBX_CORE_ENGINE_HPP_

#include <map>
#include <vector>
#include <typeindex>
#include <memory>
#include <span>
#include <string_view>
#include <cmath>
#include <chrono>
#include <ranges>

#include <libsbx/utility/concepts.hpp>
#include <libsbx/utility/noncopyable.hpp>
#include <libsbx/utility/assert.hpp>

#include <libsbx/units/time.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/application.hpp>
#include <libsbx/core/logger.hpp>
#include <libsbx/core/cli.hpp>

namespace sbx::core {

class engine : public utility::noncopyable {

  using stage = module_manager::stage;
  using module_base = module_manager::module_base;
  using module_factory = module_manager::module_factory;

public:

  engine(std::span<std::string_view> args)
  : _cli{args} {
    utility::assert_that(_instance == nullptr, "Engine already exists.");

    _instance = this;

    for (const auto& [type, factory] : module_manager::_factories()) {
      _create_module(type, factory);
    }
  }

  ~engine() {
    for (const auto& entry : _modules | std::views::reverse) {
      _destroy_module(entry.first);
    }

    _instance = nullptr;
  }

  static auto delta_time() -> units::second {
    return _instance->_delta_time;
  }

  static auto fixed_delta_time() -> units::second {
    return units::second{1.0f / 60.0f};
  }

  static auto time() -> units::second {
    return _instance->_time;
  }

  static auto quit() -> void {
    _instance->_is_running = false;
  }

  static auto cli() noexcept -> core::cli& {
    return _instance->_cli;
  }

  template<typename Module>
  requires (std::is_base_of_v<module_base, Module>)
  [[nodiscard]] static auto get_module() -> Module& {
    const auto type = std::type_index{typeid(Module)};

    if (auto entry = _instance->_modules.find(type); entry != _instance->_modules.end()) {
      return *static_cast<Module*>(entry->second);
    }

    throw std::runtime_error{fmt::format("Failed to find module '{}'", typeid(Module).name())};
  }

  auto run(std::unique_ptr<application> application) -> void {
    if (_is_running) {
      return;
    }

    using clock_type = std::chrono::high_resolution_clock;

    _is_running = true;

    auto last = clock_type::now();

    auto fixed_accumulator = units::second{};

    while (_is_running) {
      const auto now = clock_type::now();
      const auto delta_time = std::chrono::duration_cast<std::chrono::duration<std::float_t>>(now - last).count();
      last = now;


      _instance->_delta_time = units::second{delta_time};
      _instance->_time += _instance->_delta_time;

      fixed_accumulator += _instance->_delta_time;

      application->update();

      _update_stage(stage::pre);
      _update_stage(stage::normal);
      _update_stage(stage::post);

      while (fixed_accumulator >= fixed_delta_time()) {
        application->fixed_update();
        _update_stage(stage::fixed);
        fixed_accumulator -= fixed_delta_time();
      }

      _update_stage(stage::rendering);
    }
  }

private:

  auto _create_module(const std::type_index& type, const module_factory& factory) -> void {
    if (_modules.contains(type)) {
      return;
    }

    for (const auto& dependency : factory.dependencies) {
      _create_module(dependency, module_manager::_factories().at(dependency));
    }

    _modules.insert({type, std::invoke(factory.create)});
    _module_by_stage[factory.stage].push_back(type);
  }

  auto _destroy_module(const std::type_index& type) -> void {
    if (!_modules.at(type)) {
      return;
    }

    auto& factory = module_manager::_factories().at(type);

    for (const auto& dependency : factory.dependencies) {
      _destroy_module(dependency);
    }

    auto* module_instance = _modules.at(type);
    std::invoke(factory.destroy, module_instance);
    _modules.at(type) = nullptr;
  }

  auto _update_stage(stage stage) -> void {
    if (auto entry = _module_by_stage.find(stage); entry != _module_by_stage.end()) {
      for (const auto& type : entry->second) {
        _modules.at(type)->update();
      }
    }
  }

  static engine* _instance;

  units::second _delta_time;
  units::second _time;

  bool _is_running{};
  // std::vector<std::string_view> _args{};
  core::cli _cli;

  std::map<std::type_index, module_base*> _modules{};
  std::map<stage, std::vector<std::type_index>> _module_by_stage{};

}; // class engine

} // namespace sbx::core

#endif // LIBSBX_CORE_ENGINE_HPP_
