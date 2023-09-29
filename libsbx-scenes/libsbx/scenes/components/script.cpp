#include <libsbx/scenes/components/script.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>

#include <libsbx/devices/input.hpp>

namespace sbx::scenes {

script::script(const std::filesystem::path& path) {
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

auto script::invoke(const std::string& name) -> void {
  auto function = _state.get<sol::function>(name);

  if (!function.valid() || !function.is<sol::function>()) {
    // core::logger::warn("Function '{}' not found in script '{}'", name, _name);
    return;
  }

  std::invoke(function);
}

auto script::_create_bindings() -> void {
  auto library = _state.create_named_table("sbx");

  library.set_function("delta_time", [](){ return core::engine::delta_time().value(); });

  _create_logger_bindings(library);
  _create_vector3_bindings(library);
  _create_transform_bindings(library);
  _create_input_bindings(library);
}

auto script::_create_logger_bindings(sol::table& library) -> void {
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

  logger_type.set_function("error", sol::overload(
    [](const std::string& message) { core::logger::error(message); },
    [](std::float_t value) { core::logger::error(value); }
  ));
}

auto script::_create_vector3_bindings(sol::table& library) -> void {
  auto vector3_constructor = sol::constructors<math::vector3(), math::vector3(std::float_t, std::float_t, std::float_t)>{};

  auto vector3_type = library.new_usertype<math::vector3>("vector3", vector3_constructor);

  vector3_type.set("x", &math::vector3::x);
  vector3_type.set("y", &math::vector3::y);
  vector3_type.set("z", &math::vector3::z);

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
}

auto script::_create_transform_bindings(sol::table& library) -> void {
  auto transform_constructor = sol::constructors<math::transform(), math::transform(const math::vector3&, const math::vector3&, const math::vector3&)>{};

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

auto script::_create_input_bindings(sol::table& library) -> void {
  auto key_type = library.new_enum("key",
    "space", devices::key::space,
    "apostrophe", devices::key::apostrophe,
    "comma", devices::key::comma,
    "minus", devices::key::minus,
    "period", devices::key::period,
    "slash", devices::key::slash
  );

  auto input_type = library.new_usertype<devices::input>("input", sol::no_constructor);

  input_type.set_function("is_key_pressed", &devices::input::is_key_pressed);
  input_type.set_function("is_key_released", &devices::input::is_key_released);

}

} // namespace sbx::scenes
