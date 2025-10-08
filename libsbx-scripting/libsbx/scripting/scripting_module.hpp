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

namespace sbx::scripting {

class scripting_module final : public core::module<scripting_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<scenes::scenes_module>{});

public:

  scripting_module();

  ~scripting_module() override;

  auto update() -> void override;

  auto instantiate(const scenes::node node, const std::string& name) -> void;

private:

  struct script_instance {

  }; // struct script_instance

  std::unordered_map<scenes::node, std::vector<script_instance>> _instances;

}; // class scene_modules

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
