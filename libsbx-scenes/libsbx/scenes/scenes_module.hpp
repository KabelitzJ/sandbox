#ifndef LIBSBX_SCENES_SCENE_MODULE_HPP_
#define LIBSBX_SCENES_SCENE_MODULE_HPP_

#include <memory>

#include <libsbx/core/module.hpp>

#include <libsbx/audio/audio_module.hpp>
#include <libsbx/audio/sound.hpp>

#include <libsbx/scenes/scene.hpp>

#include <libsbx/scenes/components/script.hpp>

namespace sbx::scenes {

class scenes_module final : public core::module<scenes_module> {

  inline static const auto is_registered = register_module(stage::normal);

public:

  scenes_module()
  : _scene{nullptr} {
    
  }

  ~scenes_module() override = default;

  auto update() -> void override {
    if (!_scene) {
      return;
    }

    auto script_nodes = _scene->query<scenes::script>();

    for (auto& node : script_nodes) {
      _update_script(node);
    }

    auto camera_node = _scene->camera();

    const auto& camera_transform = camera_node.get_component<math::transform>();

    auto& audio_module = core::engine::get_module<audio::audio_module>();

    audio_module.update_listener_orientation(camera_transform.position(), camera_transform.forward());

    auto audio_nodes = _scene->query<audio::sound>();

    for (auto& node : audio_nodes) {
      _update_audio(node);
    }
  }

  auto load_scene(const std::filesystem::path& path) -> scenes::scene& {
    return *(_scene = std::make_unique<scenes::scene>(path));
  }

  auto scene() -> scenes::scene& {
    if (!_scene) {
      throw std::runtime_error{"No active scene"};
    }

    return *_scene;
  }

private:

  auto _update_script(scenes::node& node) -> void {
    auto& transform = node.get_component<math::transform>();

    auto& script = node.get_component<scenes::script>();

    script.set("transform", transform);

    script.invoke("on_update");

    transform = script.get<math::transform>("transform");
  }

  auto _update_audio(scenes::node& node) -> void {
    const auto& transform = node.get_component<math::transform>();
    auto& audio = node.get_component<audio::sound>();

    audio.update_orientations(transform.position(), transform.forward());
  }

  std::unique_ptr<scenes::scene> _scene;

}; // class scene_modules

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_MODULE_HPP_
