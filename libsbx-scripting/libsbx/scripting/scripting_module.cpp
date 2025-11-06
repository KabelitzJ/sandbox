#include <libsbx/scripting/scripting_module.hpp>

#include <fmt/core.h>
#include <fmt/args.h>
#include <fmt/format.h>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/tag.hpp>

#include <libsbx/scripting/interop.hpp>

namespace sbx::scripting {

static auto _push_fmt_arg(fmt::dynamic_format_arg_store<fmt::format_context>& store, const sol::object& object) -> void {
  switch (object.get_type()) {
    case sol::type::string: {
      store.push_back(object.as<std::string_view>()); 
      break;
    }
    case sol::type::number: {
      store.push_back(object.as<std::double_t>()); 
      break;
    }
    case sol::type::boolean: {
      store.push_back(object.as<bool>()); 
      break;
    }
    case sol::type::nil: {
      store.push_back("(nil)"); 
      break;
    }
    default: {
      auto state = sol::state_view{object.lua_state()};
      auto tostring = state["tostring"];
      auto result = tostring(object);

      if (result.valid()) {
        store.push_back(sol::stack::get<std::string>(result.lua_state(), -1));
      } else {
        store.push_back("<userdata>");
      }

      break;
    }
  }
}

scripting_module::scripting_module() {
  _state.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);

  _register_module();

  _register_component_type<scenes::tag>("tag");

  _register_user_types();

  auto config = scripting::managed::rumtime_config{
		.backend_path = "build/x86_64/gcc/debug/_dotnet_out",
		.exception_callback = _exception_callback
	};

  _runtime.initialize(config);

  _context = _runtime.create_assembly_load_context("ScriptingContext");

  auto core_assembly_path = std::filesystem::path{"build/x86_64/gcc/debug/_dotnet_out/Sbx.Core.dll"};
	_core_assembly = _context.load_assembly(core_assembly_path.string());

  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Log_LogMessage", reinterpret_cast<void*>(&interop::log_log_message));

  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Behavior_CreateComponent", reinterpret_cast<void*>(&interop::behavior_create_component));
  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Behavior_HasComponent", reinterpret_cast<void*>(&interop::behavior_has_component));
  // _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Behavior_RemoveComponent", reinterpret_cast<void*>(&interop::behavior_remove_component));

  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Tag_GetTag", reinterpret_cast<void*>(&interop::tag_get_tag));
  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Tag_SetTag", reinterpret_cast<void*>(&interop::tag_set_tag));

  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Input_IsKeyPressed", reinterpret_cast<void*>(&interop::input_is_key_pressed));
  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Input_IsKeyDown", reinterpret_cast<void*>(&interop::input_is_key_down));
  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Input_IsKeyReleased", reinterpret_cast<void*>(&interop::input_is_key_released));
  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Input_IsMouseButtonPressed", reinterpret_cast<void*>(&interop::input_is_mouse_button_pressed));
  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Input_IsMouseButtonDown", reinterpret_cast<void*>(&interop::input_is_mouse_button_down));
  _core_assembly.add_internal_call("Sbx.Core.InternalCalls", "Input_IsMouseButtonReleased", reinterpret_cast<void*>(&interop::input_is_mouse_button_released));

  interop::register_managed_component<scenes::tag>("Tag", _core_assembly);

  _core_assembly.upload_internal_calls();
}

scripting_module::~scripting_module() {

}

static auto demo_instance = managed::object{};

auto scripting_module::test() -> void {
  auto demo_assembly_path = std::filesystem::path{"build/x86_64/gcc/debug/_dotnet_out/Demo.dll"};
	auto& demo_assembly = _context.load_assembly(demo_assembly_path.string());

  auto demo_type = demo_assembly.get_type("Demo.Demo");

  demo_instance = demo_type.create_instance();

  auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
  auto& scene = scenes_module.scene();

  const auto node = scene.create_node("SCRIPT_TEST");

  demo_instance.set_field_value("Node", static_cast<std::uint32_t>(node));

  demo_instance.invoke("SayHello");

  demo_instance.invoke("SetTag", managed::string::create("TEST_TAG"));
  
  demo_instance.invoke("SayHello");

  demo_instance.invoke("OnCreate");
}

auto scripting_module::update() -> void {
  SBX_PROFILE_SCOPE("scripting_module::update");

  const auto delta_time = core::engine::delta_time();

  demo_instance.invoke("OnUpdate");

  for (const auto& [node, instances] : _instances) {
    for (const auto& [name, instance] : instances) {
      auto result = instance.on_update(instance.self, delta_time.value());

      if (!result.valid()) {
        utility::logger<"scripting">::error("on_create error: {}", sol::error{result}.what());
      }
    }
  }
}

auto scripting_module::instantiate(const scenes::node node, const std::filesystem::path& path) -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  const auto resolved_path = assets_module.resolve_path(path);

  utility::logger<"scripting">::info("resolved_path: {}", resolved_path.string());

  auto result = _state.safe_script_file(resolved_path.string());

   if (!result.valid()) {
    utility::logger<"scripting">::error("{}", sol::error{result}.what());
    return;
  }

  auto object = sol::object{result};

  if (!object.is<sol::table>()) {
    utility::logger<"scripting">::error("Script '{}' did not return a table!", resolved_path.string());
    return;
  }

  // auto metatable = _state.create_table_with(sol::meta_function::index, _state["behavior_base"]);

  auto instance = script_instance{};

  instance.self = object.as<sol::table>();

  _prepare_script(instance.self, node);

  if (auto handle = instance.self.get<sol::optional<sol::function>>("on_create")) {
    instance.on_create = *handle;
  }

  if (auto handle = instance.self.get<sol::optional<sol::function>>("on_update")) {
    instance.on_update = *handle;
  }

  if (auto handle = instance.self.get<sol::optional<sol::function>>("on_destroy")) {
    instance.on_destroy = *handle;
  }

  auto on_create_result = instance.on_create(instance.self);

  if (!on_create_result.valid()) {
    utility::logger<"scripting">::error("on_create error: {}", sol::error{on_create_result}.what());
  }

  _instances[node].emplace(path.stem().filename().string(), std::move(instance));
}

auto scripting_module::_register_user_types() -> void{
  _module.new_usertype<scenes::tag>("tag",
    sol::no_constructor,
    sol::meta_function::type_info, [](){
      return "tag";
    },
    sol::meta_function::to_string, [](scenes::tag& tag){
      return tag.str();
    }
  );
}

auto scripting_module::_register_module() -> void {
  _module = _state.create_table();

  _module.set_function("version", [](){ return "0.1.0"; });

  _module.set_function("debug", [](std::string_view fmt, sol::variadic_args variadic_args) {
    auto store = fmt::dynamic_format_arg_store<fmt::format_context>{};

    for (auto args : variadic_args) {
      _push_fmt_arg(store, args);
    }

    utility::logger<"scripting">::debug("{}", fmt::vformat(fmt, store));
  });

  _module.set_function("info", [](std::string_view fmt, sol::variadic_args variadic_args) {
    auto store = fmt::dynamic_format_arg_store<fmt::format_context>{};

    for (auto args : variadic_args) {
      _push_fmt_arg(store, args);
    }

    utility::logger<"scripting">::info("{}", fmt::vformat(fmt, store));
  });

  _module.set_function("warn", [](std::string_view fmt, sol::variadic_args variadic_args) {
    auto store = fmt::dynamic_format_arg_store<fmt::format_context>{};

    for (auto args : variadic_args) {
      _push_fmt_arg(store, args);
    }

    utility::logger<"scripting">::warn("{}", fmt::vformat(fmt, store));
  });

  _module.set_function("error", [](std::string_view fmt, sol::variadic_args variadic_args) {
    auto store = fmt::dynamic_format_arg_store<fmt::format_context>{};

    for (auto args : variadic_args) {
      _push_fmt_arg(store, args);
    }

    utility::logger<"scripting">::error("{}", fmt::vformat(fmt, store));
  });

  auto preload = _state["package"]["preload"];

  preload["sbx"] = [this](sol::this_state) {
    return _module;
  };
}

auto scripting_module::_prepare_script(sol::table& script, const scenes::node node) -> void {
  script.set_function("get_component", [this, node](sol::table self, sol::object which) -> sol::object {
    auto* accessor = static_cast<component_accessor*>(nullptr);
    
    if (which.get_type() != sol::type::string) {
      utility::logger<"scripting">::warn("Object is not string");

      return sol::nil; 
    }

    auto tag = which.as<std::string>();

    if (auto it = _component_accessors.find(tag); it != _component_accessors.end()) {
      accessor = &it->second;
    }

    if (!accessor) {
      return sol::nil;
    }

    auto* raw = accessor->get_raw(node);

    if (!raw) {
      return sol::nil;
    }

    return accessor->to_lua(raw, _state);
  });
}

} // namespace sbx::scripting
