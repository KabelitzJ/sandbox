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

    _state.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table, sol::lib::math, sol::lib::string);
    _state.script_file(path.string());

    _name = path.stem().string();

    _startup_function = _validate_function("startup");
    _update_function = _validate_function("update");

    _create_table();
  }

  script(const script& other) = delete;

  script(script&& other) noexcept = default;

  ~script() = default;

  auto operator=(const script& other) -> script& = delete;

  auto operator=(script&& other) noexcept -> script& = default;

  auto startup() -> void {
    std::invoke(_startup_function);
  }

  auto update(std::float_t delta_time) -> void {
    std::invoke(_update_function, delta_time);
  }
  
  template<typename Key, typename Type>
  auto set(Key&& key, const Type& value) -> void {
    _state.set(std::forward<Key>(key), value);
  }

  template<typename Key, typename Type>
  auto get(Key&& key) -> Type& {
    return _state.get<Type>(std::forward<Key>(key));
  }

private:

  auto _validate_function(const std::string& name) -> sol::function {
    auto function = _state.get<sol::function>(name);

    if (!function.valid() || !function.is<sol::function>()) {
      throw std::runtime_error{fmt::format("Could not find function '{}' in lua script '{}'", name, _name)};
    }

    return function;
  }

  auto _create_table() -> void {
    auto library = _state.create_named_table("sbx");

    auto logger_type = library.new_usertype<core::logger>("logger", sol::no_constructor);

    logger_type.set_function("debug", sol::overload(
      [](const std::string& message) { core::logger::debug(message); },
      [](std::float_t value) { core::logger::debug(value); }
    ));

    logger_type.set_function("info", sol::overload(
      [](const std::string& message) { core::logger::info(message); },
      [](std::float_t value) { core::logger::info(value); }
    ));

    logger_type.set_function("warn", sol::overload(
      [](const std::string& message) { core::logger::warn(message); },
      [](std::float_t value) { core::logger::warn(value); }
    ));

    auto vector3_constructor = sol::constructors<math::vector3(), math::vector3(std::float_t, std::float_t, std::float_t)>{};

    auto vector3_type = library.new_usertype<math::vector3>("vector3", vector3_constructor);

    vector3_type.set("x", &math::vector3::x);
    vector3_type.set("y", &math::vector3::y);
    vector3_type.set("z", &math::vector3::z);

    vector3_type.set_function("length", [](const math::vector3& vector) { return vector.length(); });

    vector3_type.set_function(sol::meta_function::addition, [](const math::vector3& lhs, const math::vector3& rhs) {
      return lhs + rhs;
    });

    vector3_type.set_function(sol::meta_function::subtraction, [](const math::vector3& lhs, const math::vector3& rhs) {
      return lhs - rhs;
    });

    vector3_type.set_function(sol::meta_function::multiplication, [](const math::vector3& lhs, std::float_t rhs) {
      return lhs * rhs;
    });

    vector3_type.set_function(sol::meta_function::division, [](const math::vector3& lhs, std::float_t rhs) {
      return lhs / rhs;
    });
  }
  
  std::string _name{};
  sol::state _state{};
  sol::function _startup_function{};
  sol::function _update_function{};

}; // class script

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPT_HPP_
