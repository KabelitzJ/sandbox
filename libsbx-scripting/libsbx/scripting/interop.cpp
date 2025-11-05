#include <libsbx/scripting/interop.hpp>

#include <libsbx/utility/logger.hpp>

namespace sbx::scripting {

auto interop::log_log_message(log_level level, managed::string message) -> void {
  switch (level) {
    case log_level::trace: {
      utility::logger<"scripting">::trace(std::string{message});
      break;
    }
    case log_level::debug: {
      utility::logger<"scripting">::debug(std::string{message});
      break;
    }
    case log_level::info: {
      utility::logger<"scripting">::info(std::string{message});
      break;
    }
    case log_level::warn: {
      utility::logger<"scripting">::warn(std::string{message});
      break;
    }
    case log_level::error: {
      utility::logger<"scripting">::error(std::string{message});
      break;
    }
    case log_level::critical: {
      utility::logger<"scripting">::critical(std::string{message});
      break;
    }
  }
}

auto interop::behavior_create_component(std::uint32_t node, managed::reflection_type component_type) -> void {

}

auto interop::behavior_has_component(std::uint32_t node, managed::reflection_type component_type) -> bool {
  auto& type = static_cast<managed::type&>(component_type);

  if (!type) {
    return false;
  }

  if (auto entry = _has_component_functions.find(type.get_type_id()); entry != _has_component_functions.end()) {
    auto func = entry->second;

    return func(static_cast<scenes::node>(node));
  }

  return false;
}

// auto interop::behavior_remove_component(std::uint32_t node, managed::reflection_type componentType) -> bool {

// }

auto interop::tag_get_tag(std::uint32_t node) -> managed::string {
  auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
  auto& scene = scenes_module.scene();

  auto& tag = scene.get_component<scenes::tag>(static_cast<scenes::node>(node));

  return managed::string::create(tag.data());
}

auto interop::tag_set_tag(std::uint32_t node, managed::string tag) -> void {
  auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
  auto& scene = scenes_module.scene();

  scene.get_component<scenes::tag>(static_cast<scenes::node>(node)) = std::string{tag};
}


} // namespace sbx::scripting
