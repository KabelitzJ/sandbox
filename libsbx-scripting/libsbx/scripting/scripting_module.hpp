#ifndef LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
#define LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_

#include <memory>
#include <unordered_map>
#include <string>

#include <sol/sol.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/logger.hpp>

#include <libsbx/async/async_module.hpp>

#include <libsbx/scripting/script.hpp>

namespace sbx::scripting {

class scripting_module : public core::module<scripting_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<async::async_module>{});

public:

  scripting_module() {

  }

  ~scripting_module() override {

  }

  auto update(std::float_t delta_time) -> void override {
    for (auto& [name, script] : _scripts) {
      script->update(delta_time);
    }
  }

  auto load_script(const std::filesystem::path& path) -> void {
    auto name = path.stem().string();

    if (_scripts.contains(name)) {
      core::logger::warn(fmt::format("Overriding existing script '{}'", name));
    }

    _scripts.insert({name, std::make_unique<scripting::script>(path)});
  }

  auto script(const std::string& name) -> script& {
    if (auto it = _scripts.find(name); it != _scripts.end()) {
      return *it->second;
    }

    throw std::runtime_error{fmt::format("Script '{}' does not exist", name)};
  }

private:

  std::unordered_map<std::string, std::unique_ptr<scripting::script>> _scripts{};

}; // class scripting_module

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_