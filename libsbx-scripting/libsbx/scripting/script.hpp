#ifndef LIBSBX_SCRIPTING_SCRIPT_HPP_
#define LIBSBX_SCRIPTING_SCRIPT_HPP_

#include <filesystem>
#include <optional>

// #define SOL_USING_CXX_LUA
// #define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/vector3.hpp>

#include <libsbx/math/transform.hpp>

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

    _name = path.stem().string();

    _state.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table, sol::lib::math, sol::lib::string);

    _create_bindings();

    _state.script_file(path.string());
  }

  script(const script& other) = delete;

  script(script&& other) noexcept = default;

  ~script() = default;

  auto operator=(const script& other) -> script& = delete;

  auto operator=(script&& other) noexcept -> script& = default;

  auto on_create() -> void {
    auto on_create = _state.get<sol::function>("on_create");

    if (!on_create.valid() || !on_create.is<sol::function>()) {
      // core::logger::warn("sbx::scripting", "Function 'on_create' not found in script '{}'", _name);
      return;
    }

    std::invoke(on_create);
  }

  auto on_update() -> void {
    auto on_update = _state.get<sol::function>("on_update");

    if (!on_update.valid() || !on_update.is<sol::function>()) {
      // core::logger::warn("sbx::scripting", "Function 'on_update' not found in script '{}'", _name);
      return;
    }

    std::invoke(on_update);
  }

  auto name() const noexcept -> const std::string& {
    return _name;
  }
  
  template<typename Key, typename Type>
  auto set(Key&& key, const Type& value) -> void {
    _state.set(std::forward<Key>(key), value);
  }

  template<typename Type, typename Key>
  auto get(Key&& key) -> sol::object {
    return _state.get<sol::object>(std::forward<Key>(key));
  }

private:

  auto _create_bindings() -> void {
    auto library = _state.create_named_table("sbx");

    auto logger_type = library.new_usertype<core::logger>("logger", sol::no_constructor);

    logger_type.set_function("debug", sol::overload(
      [](const std::string& message) { core::logger::debug("sbx::scripting", message); },
      [](std::float_t value) { core::logger::debug("sbx::scripting", value); }
    ));

    logger_type.set_function("info", sol::overload(
      [](const std::string& message) { core::logger::info("sbx::scripting", message); },
      [](std::float_t value) { core::logger::info("sbx::scripting", value); }
    ));

    logger_type.set_function("warn", sol::overload(
      [](const std::string& message) { core::logger::warn("sbx::scripting", message); },
      [](std::float_t value) { core::logger::warn("sbx::scripting", value); }
    ));

    library.set_function("delta_time", [](){ return core::engine::delta_time().value(); });

    auto vector3_constructor = sol::constructors<math::vector3(), math::vector3(std::float_t, std::float_t, std::float_t)>{};

    auto vector3_type = library.new_usertype<math::vector3>("vector3", vector3_constructor);

    vector3_type.set("x", &math::vector3::x);
    vector3_type.set("y", &math::vector3::y);
    vector3_type.set("z", &math::vector3::z);

    // vector3_type.set("zero", math::vector3::zero);

    // vector3_type["zero"] = math::vector3::zero;
    auto vector3_table = sol::table{library["vector3"]};
    vector3_table.set("zero", math::vector3::zero);

    vector3_type.set_function("normalize", &math::vector3::normalize);
    vector3_type.set_function("length", &math::vector3::length);

    vector3_type.set_function(sol::meta_function::addition, sol::overload(
      sol::resolve<math::vector3(math::vector3 lhs, const std::float_t rhs)>(&math::operator+),
      sol::resolve<math::vector3(math::vector3 lhs, const math::vector3& rhs)>(&math::operator+)
    ));

    vector3_type.set_function(sol::meta_function::subtraction, sol::overload(
      sol::resolve<math::vector3(math::vector3 lhs, const std::float_t rhs)>(&math::operator-),
      sol::resolve<math::vector3(math::vector3 lhs, const math::vector3& rhs)>(&math::operator-)
    ));

    vector3_type.set_function(sol::meta_function::multiplication, sol::overload(
      sol::resolve<math::vector3(math::vector3 lhs, const std::float_t rhs)>(&math::operator*),
      sol::resolve<math::vector3(math::vector3 lhs, const math::vector3& rhs)>(&math::operator*)
    ));

    vector3_type.set_function(sol::meta_function::division, sol::overload(
      sol::resolve<math::vector3(math::vector3 lhs, const std::float_t rhs)>(&math::operator/),
      sol::resolve<math::vector3(math::vector3 lhs, const math::vector3& rhs)>(&math::operator/)
    ));

    vector3_type.set_function(sol::meta_function::unary_minus, sol::overload(
      sol::resolve<math::vector3(const math::vector3& vector)>(&math::operator-)
    ));

    auto transform_constructor = sol::constructors<math::transform()>{};

    auto transform_type = library.new_usertype<math::transform>("transform", transform_constructor);

    transform_type.set_function("position", &math::transform::position);
    transform_type.set_function("set_position", &math::transform::set_position);
    transform_type.set_function("move_by", &math::transform::move_by);

    transform_type.set_function("euler_angles", &math::transform::euler_angles);
    transform_type.set_function("set_euler_angles", &math::transform::set_euler_angles);
    transform_type.set_function("add_euler_angles", &math::transform::add_euler_angles);

    transform_type.set_function("scale", &math::transform::scale);
    transform_type.set_function("set_scale", &math::transform::set_scale);

    transform_type.set_function("look_at", &math::transform::look_at);
  }
  
  std::string _name{};
  sol::state _state{};

}; // class script

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPT_HPP_
