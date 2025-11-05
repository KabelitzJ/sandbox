#ifndef LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
#define LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_

#include <memory>
#include <optional>
#include <utility>
#include <filesystem>
#include <unordered_map>

#include <sol/sol.hpp>

#include <libsbx/utility/hashed_string.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scripting/managed/runtime.hpp>

namespace sbx::scripting {

class scripting_module final : public core::module<scripting_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<scenes::scenes_module>{});

public:

  scripting_module();

  ~scripting_module() override;

  auto test() -> void;

  auto update() -> void override;

  auto instantiate(const scenes::node node, const std::filesystem::path& path) -> void;

private:

  static auto _exception_callback(std::string_view message) -> void {
    utility::logger<"scripting">::error("{}", message);
  }

  struct script_instance {
    sol::table self;
    sol::protected_function on_create;
    sol::protected_function on_update;
    sol::protected_function on_destroy;
  }; // struct script_instance

  struct component_accessor {
    std::function<void*(scenes::node)> get_raw;
    std::function<sol::object(void*, sol::state_view)> to_lua;
  }; // struct component_accessor

  template<class Component>
  void _register_component_type(const std::string& name) {
    _component_accessors[name] = component_accessor{
      .get_raw = [name](scenes::node node) -> void* {
        auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
        auto& scene = scenes_module.scene();

        if (!scene.has_component<Component>(node)) {
          utility::logger<"scripting">::warn("Node {} does not have component '{}'", node, name);
          return nullptr;
        }

        return &scene.get_component<Component>(node);
      },
      .to_lua = [](void* component, sol::state_view state_view) -> sol::object {
        return sol::make_object(state_view, static_cast<Component*>(component));
      }
    };
  }

  auto _register_user_types() -> void;

  auto _register_module() -> void;

  auto _prepare_script(sol::table& script, const scenes::node node) -> void;

  sol::state _state;
  sol::table _module;

  std::unordered_map<std::string, component_accessor> _component_accessors;

  std::unordered_map<scenes::node, std::unordered_map<std::string, script_instance>> _instances;

  scripting::managed::runtime _runtime;
  scripting::managed::assembly_load_context _context;
  scripting::managed::assembly _core_assembly;

}; // class scene_modules

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
