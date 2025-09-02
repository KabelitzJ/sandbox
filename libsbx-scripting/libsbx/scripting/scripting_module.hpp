#ifndef LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
#define LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_

#include <memory>
#include <optional>
#include <utility>
#include <filesystem>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

#include <libsbx/core/module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

namespace sbx::scripting {

class scripting_module final : public core::module<scripting_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<scenes::scenes_module>{});

public:

  scripting_module();

  ~scripting_module() override;

  auto update() -> void override;

  auto load_domain(const std::filesystem::path& path) -> void;

  auto load_assemblies(const std::filesystem::path& path) -> void;

private:

  auto _register_internal_calls() -> void;

  MonoDomain* _domain;
  MonoImage* _engine_image;
  MonoImage* _app_image;
  MonoObject* _instance;

}; // class scene_modules

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
