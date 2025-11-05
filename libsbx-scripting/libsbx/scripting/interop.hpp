#ifndef LIBSBX_SCRIPTING_INTEROP_HPP_
#define LIBSBX_SCRIPTING_INTEROP_HPP_

#include <functional>

#include <libsbx/utility/enum.hpp>
#include <libsbx/utility/type_name.hpp>

#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scripting/managed/string.hpp>
#include <libsbx/scripting/managed/type.hpp>

namespace sbx::scripting {

struct interop {

  enum class log_level : std::int32_t {
    trace = utility::bit_v<0>,
    debug = utility::bit_v<1>,
    info = utility::bit_v<2>,
    warn = utility::bit_v<3>,
    error = utility::bit_v<4>,
    critical = utility::bit_v<5>
  }; // enum class log_level

  static auto log_log_message(log_level level, managed::string message) -> void;

  static auto behavior_create_component(std::uint32_t node, managed::reflection_type component_type) -> void;
  static auto behavior_has_component(std::uint32_t node, managed::reflection_type component_type) -> bool;
  // static auto behavior_remove_component(std::uint32_t node, managed::reflection_type component_type) -> bool;

  static auto tag_get_tag(std::uint32_t node) -> managed::string;
  static auto tag_set_tag(std::uint32_t node, managed::string tag) -> void;

  template<typename Type>
  static auto register_managed_component(std::string_view name, managed::assembly& core_assembly) -> void {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
  
    const auto component_name = std::format("Sbx.Core.{}", name);
  
    auto& type = core_assembly.get_type(component_name);
  
    if (type) {
      _create_component_functions[type.get_type_id()] = [&scenes_module](scenes::node node) -> void { 
        auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
        auto& scene = scenes_module.scene();
  
        scene.add_component<Type>(node, "Node");
      };
      _has_component_functions[type.get_type_id()] = [&scenes_module](scenes::node node) -> bool {
        auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
        auto& scene = scenes_module.scene();
  
        return scene.has_component<Type>(node);
      };
      // _remove_component_functions[type.get_type_id()] = [&scenes_module](scenes::node node) { 
      //   auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
      //   auto& scene = scenes_module.scene();
  
      //   scene.remove_component<Type>(node);
      // };
    } else {
      utility::logger<"scripting">::warn("No C# component class found for {}!", component_name);
    }
  }

private:

  inline static auto _create_component_functions = std::unordered_map<managed::type_id, std::function<void(scenes::node)>>{};
  inline static auto _has_component_functions = std::unordered_map<managed::type_id, std::function<bool(scenes::node)>>{};
  // inline static auto _remove_component_functions = std::unordered_map<managed::type_id, std::function<void(scenes::node)>>{};

}; // class interop

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_INTEROP_HPP_