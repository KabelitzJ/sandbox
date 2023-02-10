#ifndef LIBSBX_SCRIPTING_SCRIPT_HPP_
#define LIBSBX_SCRIPTING_SCRIPT_HPP_

#include <filesystem>

#include <sol/sol.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/math/vector3.hpp>

namespace sbx::scripting {

class script {

public:

  script(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
      throw std::runtime_error{fmt::format("Could not find script '{}'", path.string())};
    }

    if (auto extension = path.extension(); extension != ".lua") {
      throw std::runtime_error{fmt::format("Script '{}' is not a valid file", path.string())};
    }

    _state.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math);
    _state.script_file(path.string());

    _name = path.stem().string();

    _startup_function = _validate_functions("startup");
    _update_function = _validate_functions("update");

    _bind_logger();
  }

  script(const script& other) = delete;

  script(script&& other) noexcept = default;

  ~script() = default;

  auto operator=(const script& other) -> script& = delete;

  auto operator=(script&& other) noexcept -> script& = default;

  auto startup() -> void {
    _startup_function.call();
  }

  auto update(std::float_t delta_time) -> void {
    _update_function.call(delta_time);
  }

  template<typename Type>
  auto set(const std::string& name, const Type& value) -> void {
    _state[name] = value;
  }

  template<typename Type>
  auto get(const std::string& name) -> Type& {
    return _state[name];
  }

private:

  auto _validate_functions(const std::string& name) -> sol::function {
    _startup_function = _state[name];

    if (!_startup_function.valid() || !_startup_function.is<sol::function>()) {
      throw std::runtime_error{fmt::format("Could not find function 'startup' in lua script '{}'", _name)};
    }

    return _startup_function;
  }

  auto _bind_logger() -> void {
    _state.set_function("debug", [](std::string message) -> void {
      core::logger::debug(message);
    });

    _state.set_function("info", [](std::string message) -> void {
      core::logger::info(message);
    });

    _state.set_function("warn", [](std::string message) -> void {
      core::logger::warn(message);
    });

    _state.set_function("error", [](std::string message) -> void {
      core::logger::error(message);
    });
  }
  
  std::string _name{};
  sol::state _state{};
  sol::function _startup_function{};
  sol::function _update_function{};

}; // class script

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPT_HPP_
