#include <libsbx/scripting/scripting_module.hpp>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

extern "C" {

struct native_node { 
  std::uint32_t id; 
}; // struct EntityNative

static auto _internal_logger_info(MonoString* string) -> void {
  char* message = mono_string_to_utf8(string);

  sbx::utility::logger<"script">::info("{}", message);

  mono_free(message);
}

static auto _internal_entity_get_name(native_node node) -> MonoString* {
  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  auto& scene = scenes_module.scene();

  const auto& tag = scene.get_component<sbx::scenes::tag>(static_cast<sbx::scenes::node>(node.id));

  return mono_string_new(mono_domain_get(), tag.c_str());
}

static auto _key_from_type(MonoReflectionType* type) -> std::string {
  auto* mt = mono_reflection_type_get_type(type);
  auto* kc = mono_class_from_mono_type(mt);
  auto* ns = mono_class_get_namespace(kc);
  auto* nm = mono_class_get_name(kc);
  
  return (ns && *ns) ? (std::string{ns} + "." + nm) : std::string{nm};
}

static auto _internal_behaviour_has_component(native_node node, MonoReflectionType* type) -> mono_bool {
  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  auto& scene = scenes_module.scene();

  const auto type_key = _key_from_type(type);

  return true;
}

static auto _internal_behaviour_get_component(native_node node, MonoReflectionType* type, void* data, int size) -> mono_bool {
  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
  auto& scene = scenes_module.scene();

  const auto type_key = _key_from_type(type);

  return true;
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

static auto _split_assembly_and_fullname(const std::string& name) -> std::pair<std::string,std::string> {
  const auto position = name.find(':');

  if (position == std::string::npos) {
    return {"", name};
  }

  return {name.substr(0, position), name.substr(position + 1)};
}

static auto _split_fullname(const std::string& fullname) -> std::pair<std::string,std::string> {
  const auto position = fullname.rfind('.');

  if (position == std::string::npos) {
    return {"", fullname};
  }

  return {fullname.substr(0, position), fullname.substr(position + 1)};
}

scripting_module::scripting_module() {
  load_domain();
}

scripting_module::~scripting_module() {
  mono_jit_cleanup(_domain);
}

auto scripting_module::update() -> void {
  SBX_SCOPED_TIMER("scripting_module");


}

auto scripting_module::load_domain() -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  mono_config_parse(nullptr);

  _domain = mono_jit_init_version("sbx_domain", "v4.0.30319");

  if (!_domain) {
    throw utility::runtime_error{"Failed to initialize domain"};
  }

  _register_internal_calls();
  _register_component_operations();
}

auto scripting_module::load_assembly(const std::string& name, const std::filesystem::path& path) -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  const auto resolved_path = assets_module.resolve_path(path);

  auto* assembly = _load_assembly(_domain, resolved_path);
  
  if (!assembly) {
    throw utility::runtime_error{"Failed to load app assembly"};
  }
  
  auto& slot = _assemblies[name];
  slot.name = name;
  slot.assembly = _load_assembly(_domain, resolved_path);
  slot.image = mono_assembly_get_image(assembly);

  utility::logger<"scripting">::debug("Loaded assembly from {}", resolved_path.string());
}

auto scripting_module::instantiate(const scenes::node node, const std::string& name) -> void {

}

auto scripting_module::_register_internal_calls() -> void {
  mono_add_internal_call("Sbx.Logger::InternalInfo", reinterpret_cast<const void*>(_internal_logger_info));

  mono_add_internal_call("Sbx.Entity::InternalGetName", reinterpret_cast<const void*>(_internal_entity_get_name));

  mono_add_internal_call("Sbx.Behaviour::InternalHasComponent", reinterpret_cast<const void*>(_internal_behaviour_has_component));
  mono_add_internal_call("Sbx.Behaviour::InternalGetComponent", reinterpret_cast<const void*>(_internal_behaviour_get_component));
}

auto scripting_module::_register_component_operations() -> void {

}

} // namespace sbx::scripting