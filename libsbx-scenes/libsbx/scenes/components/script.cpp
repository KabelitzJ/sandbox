#include <libsbx/scenes/components/script.hpp>

#include <cmath>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/devices/input.hpp>

namespace sbx::scenes {

script::script(const std::filesystem::path& path) {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  auto actual_path = assets_module.asset_path(path);

  if (!std::filesystem::exists(actual_path)) {
    throw std::runtime_error{fmt::format("Could not find script '{}'", actual_path.string())};
  }

  if (auto extension = actual_path.extension(); extension != ".lua") {
    throw std::runtime_error{fmt::format("Script '{}' is not a valid file", actual_path.string())};
  }

  _name = actual_path.stem().string();

  _state.open_libraries(sol::lib::base, sol::lib::io, sol::lib::table, sol::lib::math, sol::lib::string);

  _create_bindings();

  _state.script_file(actual_path.string());
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

  library.set("delta_time", [](){ return core::engine::delta_time().value(); });

  _create_logger_bindings(library);
  _create_vector2_bindings(library);
  _create_vector3_bindings(library);
  _create_transform_bindings(library);
  _create_input_bindings(library);
  _create_math_bindings(library);
}

auto script::_create_logger_bindings(sol::table& library) -> void {
  auto logger_type = library.new_usertype<core::logger>("logger", sol::no_constructor);
  
  logger_type.set("debug", sol::overload(
    [](const std::string& message) { core::logger::debug(message); },
    [](std::float_t value) { core::logger::debug(value); }
  ));

  logger_type.set("info", sol::overload(
    [](const std::string& message) { core::logger::info(message); },
    [](std::float_t value) { core::logger::info(value); }
  ));

  logger_type.set("warn", sol::overload(
    [](const std::string& message) { core::logger::warn(message); },
    [](std::float_t value) { core::logger::warn(value); }
  ));

  logger_type.set("error", sol::overload(
    [](const std::string& message) { core::logger::error(message); },
    [](std::float_t value) { core::logger::error(value); }
  ));
}

auto script::_create_vector2_bindings(sol::table& library) -> void {
  auto vector2_constructor = sol::constructors<math::vector2(), math::vector2(std::float_t, std::float_t)>{};

  auto vector2_type = library.new_usertype<math::vector2>("vector2", vector2_constructor);

  // vector2_type.set("x", &math::vector2::x);
  // vector2_type.set("y", &math::vector2::y);

  vector2_type.set("x", sol::property(
    [](const math::vector2& vector) { return vector.x(); },
    [](math::vector2& vector, std::float_t value) { vector.x() = value; }
  ));

  vector2_type.set("y", sol::property(
    [](const math::vector2& vector) { return vector.y(); },
    [](math::vector2& vector, std::float_t value) { vector.y() = value; }
  ));
}

auto script::_create_vector3_bindings(sol::table& library) -> void {
  auto vector3_constructor = sol::constructors<math::vector3(), math::vector3(std::float_t, std::float_t, std::float_t)>{};

  auto vector3_type = library.new_usertype<math::vector3>("vector3", vector3_constructor);

  vector3_type.set("x", sol::property(
    [](const math::vector3& vector) { return vector.x(); },
    [](math::vector3& vector, std::float_t value) { vector.x() = value; }
  ));

  vector3_type.set("y", sol::property(
    [](const math::vector3& vector) { return vector.y(); },
    [](math::vector3& vector, std::float_t value) { vector.y() = value; }
  ));

  vector3_type.set("z", sol::property(
    [](const math::vector3& vector) { return vector.z(); },
    [](math::vector3& vector, std::float_t value) { vector.z() = value; }
  ));

  // [NOTE] KAJ 2023-10-10 : SOL2 currently does not support static member variables in usertypes
  library["vector3"]["zero"] = math::vector3::zero;
  library["vector3"]["one"] = math::vector3::one;
  library["vector3"]["right"] = math::vector3::right;
  library["vector3"]["left"] = math::vector3::left;
  library["vector3"]["up"] = math::vector3::up;
  library["vector3"]["down"] = math::vector3::down;
  library["vector3"]["forward"] = math::vector3::forward;
  library["vector3"]["backward"] = math::vector3::backward;

  vector3_type.set("normalize", &math::vector3::normalize);
  vector3_type.set("length", &math::vector3::length);

  // vector3_type.set(sol::meta_function::addition, sol::overload(
  //   // [TODO] KAJ 2024-01-14 : Add support for scalar addition if needed
  //   // [](math::vector3 lhs, const std::float_t rhs) { return lhs + rhs; },
  //   [](math::vector3 lhs, const math::vector3& rhs) { return lhs + rhs; }
  // ));

  vector3_type.set(sol::meta_function::addition, [](math::vector3 lhs, const math::vector3& rhs) {
    return lhs + rhs;
  });

  vector3_type.set(sol::meta_function::subtraction, sol::overload(
    // [TODO] KAJ 2024-01-14 : Add support for scalar subtraction if needed
    // [](math::vector3 lhs, const std::float_t rhs) { return lhs - rhs; },
    [](math::vector3 lhs, const math::vector3& rhs) { return lhs - rhs; }
  ));

  vector3_type.set(sol::meta_function::multiplication, sol::overload(
    [](math::vector3 lhs, const std::float_t rhs) { return lhs * rhs; }
    // [TODO] KAJ 2024-01-14 : Add support for scalar multiplication if needed
    // [](math::vector3 lhs, const math::vector3& rhs) { return lhs * rhs; }
  ));

  vector3_type.set(sol::meta_function::division, sol::overload(
    [](math::vector3 lhs, const std::float_t rhs) { return lhs / rhs; }
    // [TODO] KAJ 2024-01-14 : Add support for scalar division if needed
    // [](math::vector3 lhs, const math::vector3& rhs) { return lhs / rhs; }
  ));

  vector3_type.set(sol::meta_function::unary_minus, sol::overload(
    [](math::vector3 vector) { return -vector; }
  ));
}

auto script::_create_transform_bindings(sol::table& library) -> void {
  auto transform_constructor = sol::constructors<math::transform(), math::transform(const math::vector3&, const math::vector3&, const math::vector3&)>{};

  auto transform_type = library.new_usertype<math::transform>("transform", transform_constructor);

  transform_type.set("position", &math::transform::position);
  transform_type.set("set_position", &math::transform::set_position);
  transform_type.set("move_by", &math::transform::move_by);

  // transform_type.set("euler_angles", &math::transform::euler_angles);
  transform_type.set("set_euler_angles", &math::transform::set_euler_angles);
  transform_type.set("add_euler_angles", &math::transform::add_euler_angles);

  transform_type.set("scale", &math::transform::scale);
  transform_type.set("set_scale", &math::transform::set_scale);

  transform_type.set("forward", &math::transform::forward);
  transform_type.set("right", &math::transform::right);

  transform_type.set("look_at", &math::transform::look_at);
}

auto script::_create_input_bindings(sol::table& library) -> void {
  auto key_type = library.new_enum<devices::key>("key", {
    { "space", devices::key::space, },
    { "apostrophe", devices::key::apostrophe, },
    { "comma", devices::key::comma, },
    { "minus", devices::key::minus, },
    { "period", devices::key::period, },
    { "slash", devices::key::slash, },
    { "left_shift", devices::key::left_shift, },
    { "up", devices::key::up, },
    { "down", devices::key::down, },
    { "left", devices::key::left, },
    { "right", devices::key::right, },
    { "w", devices::key::w, },
    { "a", devices::key::a, },
    { "s", devices::key::s, },
    { "d", devices::key::d, },
    { "q", devices::key::q, },
    { "e", devices::key::e, },
    { "r", devices::key::r}  
  });

  auto input_type = library.new_usertype<devices::input>("input", sol::no_constructor);

  input_type.set("is_key_pressed", &devices::input::is_key_pressed);
  input_type.set("is_key_released", &devices::input::is_key_released);
  input_type.set("is_key_down", &devices::input::is_key_down);

  input_type.set("is_mouse_button_pressed", &devices::input::is_mouse_button_pressed);
  input_type.set("is_mouse_button_released", &devices::input::is_mouse_button_released);
  input_type.set("is_mouse_button_down", &devices::input::is_mouse_button_down);

  input_type.set("mouse_position", &devices::input::mouse_position);

  input_type.set("scroll_delta", &devices::input::scroll_delta);
}

auto script::_create_math_bindings(sol::table& library) -> void {
  library.set("sin", sol::overload(
    [](std::float_t value) { return std::sin(value); }
  ));

  library.set("cos", sol::overload(
    [](std::float_t value) { return std::cos(value); }
  ));

  library.set("tan", sol::overload(
    [](std::float_t value) { return std::tan(value); }
  ));
}

} // namespace sbx::scenes
