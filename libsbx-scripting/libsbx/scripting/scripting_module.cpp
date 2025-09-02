#include <libsbx/scripting/scripting_module.hpp>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

extern "C" {

struct native_entity { 
  std::uint32_t id; 
}; // struct EntityNative

static auto _internal_logger_info(MonoString* string) -> void {
  char* message = mono_string_to_utf8(string);

  sbx::utility::logger<"script">::info("{}", message);

  mono_free(message);
}

static auto _internal_entity_get_name(native_entity entity) -> void {
  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  auto& scene = scenes_module.scene();
}

} // extern "C"

namespace sbx::scripting {

static auto _load_assembly(MonoDomain* domain, const std::string& path) -> MonoAssembly* {
  auto* assembly = mono_domain_assembly_open(domain, path.c_str());

  if (!assembly) {
    throw utility::runtime_error{"Failed to load assembly: {}", path};
  }

  return assembly;
}

scripting_module::scripting_module() {

}

scripting_module::~scripting_module() {
  mono_jit_cleanup(_domain);
}

auto scripting_module::update() -> void {

}

auto scripting_module::load_domain(const std::filesystem::path& path) -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  mono_config_parse(nullptr);

  _domain = mono_jit_init_version("sbx_domain", "v4.0.30319");

  if (!_domain) {
    throw utility::runtime_error{"Failed to initialize domain"};
  }

  _register_internal_calls();

  auto* assembly = _load_assembly(_domain, assets_module.resolve_path(path / "Sbx.dll"));

  if (!assembly) {
    throw utility::runtime_error{"Failed to load engine assembly"};
  }

  _engine_image = mono_assembly_get_image(assembly);
}

auto scripting_module::load_assemblies(const std::filesystem::path& path) -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  auto* assembly = _load_assembly(_domain, assets_module.resolve_path(path / "Test.dll"));

  if (!assembly) {
    throw utility::runtime_error{"Failed to load app assembly"};
  }

  _app_image = mono_assembly_get_image(assembly);
}

auto scripting_module::_register_internal_calls() -> void {
  mono_add_internal_call("Sbx.Logger::InternalInfo", reinterpret_cast<const void*>(_internal_logger_info));

  mono_add_internal_call("Sbx.Entity::InternalGetName", reinterpret_cast<const void*>(_internal_entity_get_name));
}

} // namespace sbx::scripting