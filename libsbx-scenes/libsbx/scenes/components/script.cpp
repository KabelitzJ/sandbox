#include <libsbx/scenes/components/script.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

extern "C" {

static void icall_log_info(MonoString* string) {
  char* message = mono_string_to_utf8(string);

  sbx::utility::logger<"script">::info("{}", message);

  mono_free(message);
}

} // extern "C"

namespace sbx::scenes {

static MonoDomain* domain = nullptr;
static MonoObject* instance = nullptr;

void register_internal_calls() {
  mono_add_internal_call("Sbx.Logger::Info", reinterpret_cast<const void*>(icall_log_info));
}

static auto find_method(MonoClass* klass, const std::string& name, std::uint32_t arity) -> MonoMethod* {
  auto* iterator = static_cast<void*>(nullptr);

  while (auto* method = mono_class_get_methods(klass, &iterator)) {
    if (std::string{mono_method_get_name(method)} == name) {
      const auto signature = mono_method_signature(method);
      
      if (mono_signature_get_param_count(signature) == arity) {
        return method;
      }
    }
  }

  return nullptr;
}

static auto load_assembly(const std::string& path) -> MonoAssembly* {
  auto* assembly = mono_domain_assembly_open(domain, path.c_str());

  if (!assembly) {
    throw utility::runtime_error{"Failed to load assembly: {}", path};
  }

  return assembly;
}

auto initialize_runtime() -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  mono_config_parse(nullptr);

  domain = mono_jit_init_version("sbx_domain", "v4.0.30319");

  if (!domain) {
    throw utility::runtime_error{"Failed to initialize domain"};
  }

  register_internal_calls();

  auto*     = load_assembly(assets_module.resolve_path("res://scripts/Sbx/Out/Sbx.dll"));
  auto* app_assembly = load_assembly(assets_module.resolve_path("res://scripts/Out//Test.dll"));

  if (!engine_assembly || !app_assembly) {
    throw utility::runtime_error{"Failed to load assemblies"};
  }

  auto* app_image = mono_assembly_get_image(app_assembly);

  auto* behaviour = mono_class_from_name(app_image, "", "Test");

  if (!behaviour) {
    throw utility::runtime_error{"Failed to load behaviour"};
  }

  instance = mono_object_new(domain, behaviour);
  mono_runtime_object_init(instance);


}
  
} // namespace sbx::scenes
