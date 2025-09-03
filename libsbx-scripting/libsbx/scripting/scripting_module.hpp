#ifndef LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
#define LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_

#include <memory>
#include <optional>
#include <utility>
#include <filesystem>
#include <unordered_map>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

#include <libsbx/utility/hashed_string.hpp>

#include <libsbx/core/module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

namespace sbx::scripting {

class scripting_module final : public core::module<scripting_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<scenes::scenes_module>{});

public:

  scripting_module();

  ~scripting_module() override;

  auto update() -> void override;

  auto load_domain() -> void;

  auto load_assembly(const std::string& name, const std::filesystem::path& path) -> void;

private:

  struct assembly {
    std::string name;
    MonoAssembly* assembly{nullptr};
    MonoImage* image{nullptr};
  }; // struct assembly

  struct script {
    std::string assembly;
    std::string fullname;
    MonoImage* image{nullptr};
    MonoClass* klass{nullptr};
    MonoClassField* behaviour_entity_field{nullptr};
    MonoMethod* on_create{nullptr};
    MonoMethod* on_update{nullptr};
    MonoMethod* on_destroy{nullptr};
  }; // struct script

  struct component_operations {
    std::size_t size;
    bool (*has)(sbx::scenes::scene&, sbx::scenes::node);
    bool (*get)(sbx::scenes::scene&, sbx::scenes::node, void* out, std::size_t size);
  }; // component_operations

  auto _register_internal_calls() -> void;

  auto _register_component_operations() -> void;

  MonoDomain* _domain;
  MonoImage* _engine_image;
  MonoImage* _app_image;
  MonoObject* _instance;

  std::unordered_map<utility::hashed_string, component_operations> _component_operations;
  std::unordered_map<utility::hashed_string, assembly> _assemblies;
  std::unordered_map<utility::hashed_string, script> _scripts;

}; // class scene_modules

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPTING_MODULE_HPP_
